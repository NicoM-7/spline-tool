#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

const float CLICK_TOLERANCE = 10.0f;

struct Point
{
    float x, y;
};

struct Node : public Point
{
    bool hasHandle1;
    bool hasHandle2;
    Point handle1;
    Point handle2;

    Node(float x_val, float y_val)
    {
        x = x_val;
        y = y_val;
        hasHandle1 = false;
        hasHandle2 = false;
    }
};

std::vector<Node> nodes;

bool draggingNode = false;
int draggedNodeIndex = -1;
float nodeDragOffsetX = 0.0f, nodeDragOffsetY = 0.0f;

bool draggingHandle = false;
int draggedHandleNodeIndex = -1;
int draggedHandleType = 0;

float distancef(float x1, float y1, float x2, float y2)
{
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

Point evaluateBezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, float t)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Point p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    return p;
}

void renderSpline()
{
    if (nodes.size() < 2)
        return;

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < nodes.size() - 1; ++i)
    {
        Point p0 = {nodes[i].x, nodes[i].y};
        Point p3 = {nodes[i + 1].x, nodes[i + 1].y};

        Point p1 = (nodes[i].hasHandle2) ? nodes[i].handle2 : p0;
        Point p2 = (nodes[i + 1].hasHandle1) ? nodes[i + 1].handle1 : p3;

        glBegin(GL_LINE_STRIP);
        for (int j = 0; j <= 200; ++j)
        {
            float t = j / 200.0f;
            Point pt = evaluateBezier(p0, p1, p2, p3, t);
            glVertex2f(pt.x, pt.y);
        }
        glEnd();
    }
    glDisable(GL_LINE_SMOOTH);
}

void renderNodes()
{
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_POINT_SMOOTH);
    glPointSize(12.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto &node : nodes)
    {
        glVertex2f(node.x, node.y);
    }
    glEnd();
    glEnable(GL_MULTISAMPLE);
}

void renderHandles()
{
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto &node : nodes)
    {
        if (node.hasHandle1)
            glVertex2f(node.handle1.x, node.handle1.y);
        if (node.hasHandle2)
            glVertex2f(node.handle2.x, node.handle2.y);
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}

void renderDottedLines()
{
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glLineWidth(1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (const auto &node : nodes)
    {
        if (node.hasHandle1)
        {
            glVertex2f(node.x, node.y);
            glVertex2f(node.handle1.x, node.handle1.y);
        }
        if (node.hasHandle2)
        {
            glVertex2f(node.x, node.y);
            glVertex2f(node.handle2.x, node.handle2.y);
        }
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void addNewNode(float x, float y)
{
    Node newNode(x, y);

    if (nodes.empty())
    {
        newNode.hasHandle2 = true;
        newNode.handle2 = {x, y + 50.0f};
        nodes.push_back(newNode);
    }
    else if (nodes.size() == 1)
    {
        newNode.hasHandle1 = true;
        newNode.handle1 = {x, y + 50.0f};
        nodes.push_back(newNode);
    }
    else
    {
        Node &firstNode = nodes.front();
        Node &lastNode = nodes.back();
        float distFirst = distancef(x, y, firstNode.x, firstNode.y);
        float distLast = distancef(x, y, lastNode.x, lastNode.y);
        if (distFirst < distLast)
        {
            if (!firstNode.hasHandle1 && firstNode.hasHandle2)
            {
                firstNode.hasHandle1 = true;
                firstNode.handle1.x = 2 * firstNode.x - firstNode.handle2.x;
                firstNode.handle1.y = 2 * firstNode.y - firstNode.handle2.y;
            }
            newNode.hasHandle2 = true;
            newNode.handle2 = {x, y + 50.0f};
            nodes.insert(nodes.begin(), newNode);
        }
        else
        {
            if (!lastNode.hasHandle2 && lastNode.hasHandle1)
            {
                lastNode.hasHandle2 = true;
                lastNode.handle2.x = 2 * lastNode.x - lastNode.handle1.x;
                lastNode.handle2.y = 2 * lastNode.y - lastNode.handle1.y;
            }
            newNode.hasHandle1 = true;
            newNode.handle1 = {x, y + 50.0f};
            nodes.push_back(newNode);
        }
    }
}

int main(int argc, char **argv)
{
    int screenWidth = 800, screenHeight = 600;
    if (argc >= 3)
    {
        screenWidth = std::atoi(argv[1]);
        screenHeight = std::atoi(argv[2]);
    }

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Spline Tool", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return -1;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool prevLeftMouseDown = false;

    while (!glfwWindowShouldClose(window))
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        float worldMouseX = static_cast<float>(mouseX);
        float worldMouseY = screenHeight - static_cast<float>(mouseY);

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            nodes.clear();
            draggingNode = false;
            draggingHandle = false;
        }

        int leftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

        if (leftState == GLFW_PRESS && !prevLeftMouseDown)
        {
            bool foundInteraction = false;
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                if (nodes[i].hasHandle1 &&
                    distancef(worldMouseX, worldMouseY, nodes[i].handle1.x, nodes[i].handle1.y) < CLICK_TOLERANCE)
                {
                    draggingHandle = true;
                    draggedHandleNodeIndex = i;
                    draggedHandleType = 1;
                    foundInteraction = true;
                    break;
                }
                if (nodes[i].hasHandle2 &&
                    distancef(worldMouseX, worldMouseY, nodes[i].handle2.x, nodes[i].handle2.y) < CLICK_TOLERANCE)
                {
                    draggingHandle = true;
                    draggedHandleNodeIndex = i;
                    draggedHandleType = 2;
                    foundInteraction = true;
                    break;
                }
            }
            if (!foundInteraction)
            {
                for (size_t i = 0; i < nodes.size(); ++i)
                {
                    if (distancef(worldMouseX, worldMouseY, nodes[i].x, nodes[i].y) < CLICK_TOLERANCE)
                    {
                        draggingNode = true;
                        draggedNodeIndex = i;
                        nodeDragOffsetX = nodes[i].x - worldMouseX;
                        nodeDragOffsetY = nodes[i].y - worldMouseY;
                        foundInteraction = true;
                        break;
                    }
                }
            }
            if (!foundInteraction)
            {
                addNewNode(worldMouseX, worldMouseY);
            }
        }

        if (leftState == GLFW_PRESS)
        {
            if (draggingNode && draggedNodeIndex >= 0 && draggedNodeIndex < (int)nodes.size())
            {
                float newPosX = worldMouseX + nodeDragOffsetX;
                float newPosY = worldMouseY + nodeDragOffsetY;
                float dx = newPosX - nodes[draggedNodeIndex].x;
                float dy = newPosY - nodes[draggedNodeIndex].y;
                nodes[draggedNodeIndex].x = newPosX;
                nodes[draggedNodeIndex].y = newPosY;
                if (nodes[draggedNodeIndex].hasHandle1)
                {
                    nodes[draggedNodeIndex].handle1.x += dx;
                    nodes[draggedNodeIndex].handle1.y += dy;
                }
                if (nodes[draggedNodeIndex].hasHandle2)
                {
                    nodes[draggedNodeIndex].handle2.x += dx;
                    nodes[draggedNodeIndex].handle2.y += dy;
                }
            }
            if (draggingHandle && draggedHandleNodeIndex >= 0 && draggedHandleNodeIndex < (int)nodes.size())
            {
                Node &node = nodes[draggedHandleNodeIndex];
                if (draggedHandleType == 1 && node.hasHandle1)
                {
                    node.handle1.x = worldMouseX;
                    node.handle1.y = worldMouseY;
                    if (node.hasHandle2)
                    {
                        node.handle2.x = 2 * node.x - node.handle1.x;
                        node.handle2.y = 2 * node.y - node.handle1.y;
                    }
                }
                if (draggedHandleType == 2 && node.hasHandle2)
                {
                    node.handle2.x = worldMouseX;
                    node.handle2.y = worldMouseY;
                    if (node.hasHandle1)
                    {
                        node.handle1.x = 2 * node.x - node.handle2.x;
                        node.handle1.y = 2 * node.y - node.handle2.y;
                    }
                }
            }
        }
        else
        {
            draggingNode = false;
            draggingHandle = false;
            draggedNodeIndex = -1;
            draggedHandleNodeIndex = -1;
            draggedHandleType = 0;
        }

        prevLeftMouseDown = (leftState == GLFW_PRESS);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        renderSpline();
        renderDottedLines();
        renderNodes();
        renderHandles();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}