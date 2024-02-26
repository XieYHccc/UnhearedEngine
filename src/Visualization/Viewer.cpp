#include "Viewer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Viewer::Viewer(const char* title, int width, int height)
:Window(title, width, height), cam_(glm::vec3(0.0f, 0.0f, 10.0f)) {

    lastX_ = 0.f;
    lastY_ = 0.f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    first_mouse_ = true;
    
}
Viewer::~Viewer() {

}
void Viewer::process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam_.ProcessKeyboard(FORWARD, deltaTime_);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam_.ProcessKeyboard(BACKWARD, deltaTime_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam_.ProcessKeyboard(LEFT, deltaTime_);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam_.ProcessKeyboard(RIGHT, deltaTime_);
}

void Viewer::motion(double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse_)
    {
        lastX_ = xpos;
        lastY_ = ypos;
        first_mouse_ = false;
    }

    float xoffset = xpos - lastX_;
    float yoffset = lastY_ - ypos; // reversed since y-coordinates go from bottom to top

    lastX_ = xpos;
    lastY_ = ypos;

    cam_.ProcessMouseMovement(xoffset, yoffset);
}

void Viewer::render() {

    if (!renderer.valid()) {
        return;
    }
    // per-frame time logic
    float currentFrame;
    frame_time(currentFrame);
    deltaTime_ = currentFrame - lastFrame_;
    lastFrame_ = currentFrame;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(cam_.Zoom), (float)width() / height(), 0.1f, 100.0f);
    glm::mat4 view = cam_.GetViewMatrix();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    glm::mat4 mvp = projection * view * model;

    renderer.set_model_matrix(model);
    renderer.set_view_matrix(view);
    renderer.set_projection_matrix(projection);
    renderer.render();

}