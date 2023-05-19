#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <math.h>

#define WIDTH 320
#define HEIGHT 200

SDL_Window* window;
SDL_GLContext context;
GLuint program;
GLuint vao;
GLuint vbo;
GLuint texture;

int offset;
int offset2;

GLfloat colors[WIDTH * HEIGHT * 4];  // Array of colors (RGB format)

const GLchar* vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec2 position;\n"
    "out vec2 texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);\n"
    "    texCoord = position;\n"
    "}\n";

const GLchar* fragmentShaderSource =
    "#version 330 core\n"
    "uniform sampler2D colorTexture;\n"
    "in vec2 texCoord;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "    fragColor = texture(colorTexture, texCoord);\n"
    "}\n";

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void setupShaders()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupBuffers()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void setupTexture()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void updateTexture()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_FLOAT, colors);
}

void render()
{
    updateTexture();
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "colorTexture"), 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void cleanup()
{
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texture);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawSineWave(float colorArray[], int width, int height, int amplitude, float frequency, float of) {
    float increment = 2.0 * 3.14 / width;
    int midPoint = height / 2;
    int x, y;

    for (x = 0; x < width; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of));
        y += midPoint;

        if (y >= 0 && y < height) {
            int pixelIndex = (y * width + x) * 4; // Assuming RGBA format

            // Set the color values for the pixel
            colorArray[pixelIndex] = 1.0;     // Red component
            colorArray[pixelIndex + 1] = 0.0; // Green component
            colorArray[pixelIndex + 2] = 0.0; // Blue component
            colorArray[pixelIndex + 3] = 1.0; // Alpha component
        }
    }
}

void clearFramebuffer()
{
    for (int i = 0; i < WIDTH * HEIGHT * 4; i++)
    {
        colors[i] = 0.0f;
    }
}

void handleInput(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_UP)
        {
            offset++;
            printf("%d\n", offset);
        }
        else if (event.key.keysym.sym == SDLK_DOWN)
        {
            offset--;
            printf("%d\n", offset);
        }
        else if (event.key.keysym.sym == SDLK_LEFT)
        {
            offset2--;
            printf("%d\n", offset2);
        }
        else if (event.key.keysym.sym == SDLK_RIGHT)
        {
            offset2++;
            printf("%d\n", offset2);
        }
    }
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Framebuffer Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(glewError));
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    setupShaders();
    setupBuffers();
    setupTexture();

    offset2 = 30;
    offset = 30;

    float t = 0.0f;
    while (1)
    {
        t = t + 0.05;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            handleInput(event);
            if (event.type == SDL_QUIT)
            {
                cleanup();
                return 0;
            }
        }

        glViewport(0, 0, SDL_GetWindowSurface(window)->w, SDL_GetWindowSurface(window)->h);
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        SDL_GL_SwapWindow(window);
        clearFramebuffer();
        drawSineWave(colors, WIDTH, HEIGHT, 10 + offset2, 0.1f * offset, t);

        /*int block = ((WIDTH * HEIGHT) * 4) / 32;
        for (int d = 0; d < (WIDTH * HEIGHT * 4); d = d + block)
        {
            float col = 0.01f;
            GLfloat buf[block];
            for (int i = 0; i < block; i = i + 4)
            {
                buf[i] = col * offset;
                buf[i + 1] = col * -offset;
                buf[i + 2] = col * (i + offset);
                buf[i + 3] = 1.0f;
            }
            //printf("%d\n", d);
            memcpy(colors + d, buf, sizeof(float) * block);
        } */
    }

    return 0;
}
