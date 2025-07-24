#include <print>
#include <fstream>
#include <array>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

static constexpr int WIDTH  = 1600;
static constexpr int HEIGHT = 900;

struct Vertex {
    glm::vec3 m_pos;
    Vertex(glm::vec3 pos) : m_pos(pos) { }
};

[[nodiscard]] static std::string read_entire_file(const char* filename) {
    std::ifstream file(filename);
    return std::string((std::istreambuf_iterator<char>(file)),
                       (std::istreambuf_iterator<char>()));
}

[[nodiscard]] static
GLuint create_shader_program(const char* vertex_src, const char* fragment_src) {

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    std::string vert_src = read_entire_file(vertex_src);
    auto vert_cstr = vert_src.c_str();
    glShaderSource(vert, 1, &vert_cstr, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    std::string frag_src = read_entire_file(fragment_src);
    auto frag_cstr = frag_src.c_str();
    glShaderSource(frag, 1, &frag_cstr, nullptr);
    glCompileShader(frag);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);

    return program;
}

static void process_inputs(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

[[nodiscard]] static GLFWwindow* setup_glfw() {
    glfwSetErrorCallback([]([[maybe_unused]] int error_code, char const* desc) {
        std::println(stderr, "GLFW Error: {}", desc);
    });

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "gl", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL(glfwGetProcAddress);
    glfwSetFramebufferSizeCallback(
        window, []([[maybe_unused]] GLFWwindow* win, int w, int h) {
            glViewport(0, 0, w, h);
        }
    );

    return window;
}

int main() {

    std::array vertices {
        Vertex({ -1, -1, 0.0f }), // bottom-left
        Vertex({  1, -1, 0.0f }), // bottom-right
        Vertex({  1,  1, 0.0f }), // top-right
        Vertex({ -1,  1, 0.0f }), // top-left
    };

    auto indices = std::to_array<unsigned int>({
        0, 1, 2,
        3, 0, 2
    });

    GLFWwindow* window = setup_glfw();

    glDebugMessageCallback([](
        [[maybe_unused]] GLenum src,
        [[maybe_unused]] GLenum type,
        [[maybe_unused]] GLuint id,
        [[maybe_unused]] GLenum severity,
        [[maybe_unused]] GLsizei len,
        const char* msg,
        [[maybe_unused]] const void* args
    ) { std::println(stderr, "OpenGL Error: {}", msg); }, nullptr);

    GLuint program = create_shader_program("shader.vert", "shader.frag");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    GLuint a_pos = glGetAttribLocation(program, "a_pos");
    void* offset = reinterpret_cast<void*>(offsetof(Vertex, m_pos));
    glVertexAttribPointer(a_pos, 3, GL_FLOAT, false, sizeof(Vertex), offset);
    glEnableVertexAttribArray(a_pos);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        glUniform1f(glGetUniformLocation(program, "u_time"), glfwGetTime());
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glUniform2f(glGetUniformLocation(program, "u_resolution"), width, height);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        process_inputs(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
