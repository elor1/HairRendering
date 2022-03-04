#include "Application.h"

int main(void)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    Application* app = new Application(1920, 1080);
    app->Run();
   
    delete app;    
    glfwTerminate();
    return 0;
}