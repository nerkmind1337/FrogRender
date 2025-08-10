#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* userparam) {
    fprintf( stderr, "GL CALLBACK: %s source=0x%x type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            source,type, severity, msg );
}

int main() {
    int window_width = 800, window_height = 800;
    //init glfw lib and create window
    glfwInit();
    //glfw doenst know the version of opengl we are using, so we hint it -- looks like the project still compiles and runs fine without these hints... i wonder what issues we will run into without it.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //major version 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); //minor version 5, so 4.5
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
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); //load the configurations for openGL.

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorCallback, 0);
    glViewport(0, 0, window_width, window_height);


    //SHADER SETUP
    //since all openGL objects are accessed by reference - lets create a place to store our shader in!
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //GLuint is the openGL version of an unsigned int
    
    //the reference value||string count||shader source||no idea. but its null.
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    
    //compile shader to machine code
    glCompileShader(vertexShader);
    
    //same process, but for the fragment shader.
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    
    glCompileShader(fragmentShader);
    
    //Wrap the shaders in the shader program - we need to create a shader program in order to process and execute the shaders.
    GLuint shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertexShader);
    
    glAttachShader(shaderProgram, fragmentShader);
    
    glLinkProgram(shaderProgram);
    
    //now that we have loaded the shaders in the shader program, and wrapped the program. We no longer need the shaders we created before-
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    const GLfloat vertices[] =
    {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lower left corner
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lower right corner
        0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f // Upper corner
    };


    //creating VBO to pass data between CPU and GPU - as passing data between these two is relatively slow, it's better to send big batches of data at once, in something called a "Vertex Buffer Object"
    //or, VBO.
    GLuint VAO,VBO; //typically it would be an array of references GLuint[] - but since we only have one object..this makes more sense currently
    // n = number of objects included in the VBO batch, it's worth noting that typically you'll send an array of VBO's,  but we only have one so...
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //binding time, Binding - in the context of opengl, means that we make the target object the "current" object.
    //Meaning whenever we invoke a function that would mutate that type of object, it is the currently "selected", or BOUND object that will be mutated
    //GL_ARRAY_BUFFER is just one of many type of buffer - in this case, we're sending a VertexBufferObject.
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //all the binding is done, but opengl still doesnt know how to read the VBO data, we need to configure it.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);// a vertex attribute is a way to communicate with a vertex shader from the outside of the shader program, post wrapping!
    glEnableVertexAttribArray(0);

    //now that we've bound our buffer, selecting it for operation, we'll actually store our vertex data in the buffer.
    //type_of_buffer || total size of data to go in the  buffer || the data its self || how it will be used - this is specified as a sort of "hint" to improve performance
    //1. GL_STREAM_DRAW:  the vertices will be modified once, and used a few times
    //2. GL_STATIC_DRAW:   the vertices will be modified once, and used MANY times
    //3. GL_DYNAMIC_DRAW:   the vertices will be modified many times, and used MANY times
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //so since we're done working with this VBA and VBO...let's unbind them so we dont accidently mutate them later, if we create a new object and forget to bind it...
    glBindBuffer(GL_ARRAY_BUFFER, 0); //although this sort of...doesnt "unbind" the buffer so much as it just removes the current buffer and replaces it with an empty buffer.
    glBindVertexArray(0); //same here, note the order is reversed from creation! - you must always create the array to contain the buffer first, and you must always remove the buffer from the array before deleting the array. Makes sense if you think about it.



    //ok lets load some graphics- each frame is displayed by changing the pixels of a display to the pixels of a frame, one by one from the top left of the screen to the bottom right.
    //while loading and displaying the current frame or "buffer" as i should refer to it in this context, it will be writing the next one in the back ground. and the cycle repeats.
    //the buffer that is currently being read and displayed is known as the "front buffer" the buffer that is currently being prepared and written to in the background is the "back buffer"
    //ok lets do some rendering...
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //this sets  the color of the next "clear" to the given rbga values
    glClear(GL_COLOR_BUFFER_BIT); //this performs the actual clear, with the colour above -- what does this "mask" do? what is a "GL_COLOR_BUFFER_BIT"


    //ok so now the backbuffer has been set to the colour we want, and the front buffer is being displayed with the default colour
    //so in order to see our change, we'll need to "Swap the buffers"


    //ok triangle time, but first a short explainer on the grapgics pipeline.
    // So, basically it's just a series of functions that execute using the result of the previous functions return value (excluding the first step in the pipeline, obvously)
    // The initial input to the pipeline is known as the "Vertex Data[]" - it is an array, but not of mathematical vertices as you might imagine...while it DOES contain traditional vertices, it also contains
    //mixed data ABOUT the vertices, such as colour. - so it is more accurate to describe it as "An array containing data which describes vertices,hence, "Vertex Data"
    //the pileline goes as such
    /*
    +-----------------+       +------------------+       +-----------------+
    |  Vertex Data[]  | ----> |  Vertex Shader   | ----> | Geometry Shader |
    +-----------------+       +------------------+       +-----------------+
                                                            |
                                                            v
                                                   +-----------------+
                                                   | Shape Assembly  |
                                                   +-----------------+
                                                            |
                                                            v
                                                   +-----------------+
                                                   |  Rasterization  |
                                                   +-----------------+
                                                            |
                                                            v
                                                   +-----------------+
                                                   | Fragment Shader |
                                                   +-----------------+
                                                            |
                                                            v
                                                   +-----------------+
                                                   | Tests & Blending|
                                                   +-----------------+
*/

    //1. the vertex shader, take all the positions of the vertices and transforms them, OR it can keep them the same. dealers choice
    //2. the shape assembler, takes all the positions and connects them according to a primitive™, a primitive™ is just a base shape such as a triangle, a point or a line. each causes a different interpritation of the data in the vertex data []
    //3. then we have the geometry shader, this can add vertecies and create new primitives from existing primitives, for example adding two triangles together to create a square.
    //4. then, rasterization phase, this takes the perfect™ mathematical representation of a shape, and expresses it as pixels.
    //5. then the fragment shader, this adds colours. dependant on lighting, shadows, and many other options.due to these potentially overlapping factors, you can end up with many colours for one pixel, this is fixed in the final step of the pipeline
    //6. Tests and blending - this blends the transparant objects together into the final colour.
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //this sets  the color of the next "clear" to the given rbga values
        glClear(GL_COLOR_BUFFER_BIT); //this performs the actual clear, with the colour above -- what does this "mask" do? what is a "GL_COLOR_BUFFER_BIT"

        //activate shader Program...does this really need to be done on each frame??..I guess you may want to use different shader programs for different circumstances so sure..
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); //bind vertex array
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window); //remember to swap the buffers! or it's all for nothing...
        glfwPollEvents(); //process events from window
    }

    //cleanup objects
     glDeleteVertexArrays(1, &VAO);
     glDeleteBuffers(1, &VBO);
     glDeleteProgram(shaderProgram);

    //terminate
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}