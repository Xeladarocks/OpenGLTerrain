

void centerWindow(GLFWwindow* window, GLFWmonitor* monitor) {
    if (!monitor)
        return;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
        return;

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glfwSetWindowPos(window,
        monitorX + (mode->width - windowWidth) / 2,
        monitorY + (mode->height - windowHeight) / 2);
}

#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define MIN(a, b) (((b) < (a)) ? (b) : (a))
GLFWmonitor* getBestMonitor(GLFWwindow* window) {
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    if (!monitors)
        return NULL;

    int windowX, windowY, windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetWindowPos(window, &windowX, &windowY);

    GLFWmonitor* bestMonitor = NULL;
    int bestArea = 0;

    for (int i = 0; i < monitorCount; ++i)
    {
        GLFWmonitor* monitor = monitors[i];

        int monitorX, monitorY;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!mode)
            continue;

        int areaMinX = MAX(windowX, monitorX);
        int areaMinY = MAX(windowY, monitorY);

        int areaMaxX = MIN(windowX + windowWidth, monitorX + mode->width);
        int areaMaxY = MIN(windowY + windowHeight, monitorY + mode->height);

        int area = (areaMaxX - areaMinX) * (areaMaxY - areaMinY);

        if (area > bestArea)
        {
            bestArea = area;
            bestMonitor = monitor;
        }
    }

    return bestMonitor;
}

glm::vec3 HSVtoRGB(float H, float S, float V) {
    float C = S * V;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = V - C;
    float Rs, Gs, Bs;
    if (H >= 0 && H < 60) {
        Rs = C;
        Gs = X;
        Bs = 0;
    } else if (H >= 60 && H < 120) {
        Rs = X;
        Gs = C;
        Bs = 0;
    } else if (H >= 120 && H < 180) {
        Rs = 0;
        Gs = C;
        Bs = X;
    } else if (H >= 180 && H < 240) {
        Rs = 0;
        Gs = X;
        Bs = C;
    } else if (H >= 240 && H < 300) {
        Rs = X;
        Gs = 0;
        Bs = C;
    } else {
        Rs = C;
        Gs = 0;
        Bs = X;
    }
    return glm::vec3(
        (Rs + m), 
        (Gs + m), 
        (Bs + m));
}