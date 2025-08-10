#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    int window_width = 800, window_height = 800;
    //init glfw lib and create window
    glfwInit();
    //glfw doenst know the version of opengl we are using, so we hint it -- looks like the project still compiles and runs fine without these hints... i wonder what issues we will run into without it.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //minor version also 3, so 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // this will only load the modern functions, people with older graphics cards / drivers will *possibly* be unable to use frog render :'(

    //ok lets create the window
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Frog Simulator", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    //apparently creating the window isn't enough, we also need to actually set our window as the "current context"
    glfwMakeContextCurrent(window); // <--- this function tells glfw to make the window part of the current context. Note: the context is global. Just like everything else in opengl apparantly.

    //glad configuration
    gladLoadGL(); //load the configurations for openGL.

    glViewport(0, 0, window_width, window_height);


    //ok lets load some graphics- each frame is displayed by changing the pixels of a display to the pixels of a frame, one by one from the top left of the screen to the bottom right.
    //while loading and displaying the current frame or "buffer" as i should refer to it in this context, it will be writing the next one in the back ground. and the cycle repeats.
    //the buffer that is currently being read and displayed is known as the "front buffer" the buffer that is currently being prepared and written to in the background is the "back buffer"
    //todo: question, is it a library specific to write pixels from top left to bottom right? or is that some kind of standard.
    //todo: question, is it possible to mutate the front buffer? would it even make sense to do this if you could?
    //ok lets do some rendering...

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //this sets  the color of the next "clear" to the given rbga values
    glClear(GL_COLOR_BUFFER_BIT); //this performs the actual clear, with the colour above -- what does this "mask" do? what is a "GL_COLOR_BUFFER_BIT"


    //ok so now the backbuffer has been set to the colour we want, and the front buffer is being displayed with the default colour
    //so in order to see our change, we'll need to "Swap the buffers"
    glfwSwapBuffers(window); //this relates to my question earlier..is it possible to just mutate the front buffer BEFORE we get to this while loop..in order to avoid the buffer swap? a bit pointless
    //but curious

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); //process events from window

    }
    //terminate
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}