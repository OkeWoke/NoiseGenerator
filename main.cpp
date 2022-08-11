#include <iostream>
#include "opencv2/opencv.hpp"
#include <string>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "INoiseGen2D.h"
#include "ValueNoise2D.h"


struct layer
{
    std::unique_ptr<INoiseGen2D> noise;
    float frequency = 0.5;
    float amplitude = 0.5;
};

void LoadTexture(const float* raw_data, GLuint* out_texture, int out_width, int out_height)
{

    char image_data[out_width*out_height];
    for(int i=0; i< out_height*out_width; i++)
    {
        image_data[i] = static_cast<unsigned char>(raw_data[i]);
    }
    // Load from file
    int image_width = 0;
    int image_height = 0;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(GL_TEXTURE_2D, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);

    *out_texture = image_texture;
}


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main() {
    std::vector<layer> noise_layers;

    std::cout << "Noise Generator" << std::endl;
    const unsigned int WIDTH = 500;
    const unsigned int HEIGHT = 500;
    const unsigned int WIN_WIDTH = 1000;
    const unsigned int WIN_HEIGHT = 1000;
    float floatImage[WIDTH*HEIGHT] = {};
    unsigned char renderImage[WIDTH*HEIGHT] = {};

    const unsigned int SEED = 1234;
    const unsigned NUM_LAYERS = 10;
    const float MAX_AMPLITUDE = 1.0f;
    const float MAX_FREQUENCY = 0.8f;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Noise Generator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    //texture
    GLuint myTexture = 0;
    LoadTexture(floatImage, &myTexture, WIDTH, HEIGHT);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Noise Layer Editor");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Add/Remove noise layers and edit parameters");               // Display some text (you can use a format strings too)
            //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            unsigned idxToDel = -1;
            for (unsigned idx=0; idx<noise_layers.size(); idx++)
            {
                ImGui::PushID(idx);
                std::ostringstream oss;
                oss << "Layer: " << idx;
                ImGui::Text(oss.str().c_str());

                ImGui::SliderFloat("Frequency", &noise_layers[idx].frequency, 0.0f, MAX_FREQUENCY);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::SliderFloat("Amplitude", &noise_layers[idx].amplitude, 0.0f, MAX_AMPLITUDE);
                if (ImGui::Button(" - "))
                {
                    idxToDel = idx;
                }

                ImGui::PopID();
            }

            if (idxToDel != -1)
            {
                noise_layers.erase(noise_layers.begin()+idxToDel);
            }

            if (ImGui::Button("+ Layer"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                layer l;
                l.noise = std::unique_ptr<INoiseGen2D>(new ValueNoise2D());
                l.amplitude = MAX_AMPLITUDE;
                l.frequency = MAX_FREQUENCY;
                noise_layers.push_back(std::move(l));
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        {
            ImGui::Begin("OpenGL Texture Text");
            ImGui::Text("pointer = %p", myTexture);
            ImGui::Text("size = %d x %d", WIDTH, HEIGHT);
            ImGui::Image((void*)(intptr_t)myTexture, ImVec2(WIDTH, HEIGHT));
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;


    cv::Mat image(HEIGHT, WIDTH, CV_8UC1);
    ValueNoise2D noiser(SEED);


    for(unsigned int x = 0; x < WIDTH; x++)
    {
        for(unsigned int y = 0; y < HEIGHT; y++)
        {
            float value = 0;
            float frequency = 0.005f; // smaller this number, the lower the frequency and thus it maps as larger structure
            const float max_amplitude = 10.0f;
            float denominator = max_amplitude;
            float amplitude = max_amplitude;
            for(unsigned l = 0; l < NUM_LAYERS; l++)
            {
                value+= amplitude * noiser.sample(x * frequency, y * frequency);
                denominator+=amplitude;
                frequency*=2;
                amplitude/=1.5;
            }



            image.at<unsigned char>(y, x) = static_cast<int>(255*value/denominator);
        }
    }

    cv::imshow("Noise Generator", image);
    cv::waitKey(0);
    cv::imwrite("ValueNoiseMap.png", image);

    return 0;
}
