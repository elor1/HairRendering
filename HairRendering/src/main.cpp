#include "Application.h"

int main(void)
{
    Application* app = new Application(1920, 1080);
    app->Run();

   glfwTerminate();
    return 0;
}