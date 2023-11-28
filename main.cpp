#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"imgui_impl_opengl3_loader.h"

#include<GLFW/glfw3.h>
#include<iostream>

#include"Model.h"
#include"Shader.h"

const unsigned int height = 800;
const unsigned int width = 800;

bool showTestWindow = true;
bool showTest2Window = false;
bool showTest3Window = false;

void jed();
void dwa();
void trzy();

void jed()
{
	ImGui::Begin("Test", &showTestWindow);
	ImGui::Text("Lorem ipsum");
	if (ImGui::Button("Next"))
	{
		showTestWindow = false;
		showTest2Window = true;
	}
	if (ImGui::Button("Next"))
	{
		showTestWindow = false;
		showTest3Window = true;
	}
	ImGui::End();
}

void dwa() 
{
	ImGui::Begin("Test 2", &showTest2Window);
	ImGui::Text("Lorem Ipsum");
	if (ImGui::Button("Back"))
	{
		showTest2Window = false;
		showTestWindow = true;
	}
	ImGui::End();
}
void trzy() 
{
	ImGui::Begin("Test 3", &showTest3Window);
	ImGui::Text("Lorem Ipsum");
	if (ImGui::Button("Back"))
	{
		showTest3Window = false;
		showTestWindow = true;
	}
	ImGui::End();
}
int main()
{
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(width, height, "window", NULL, NULL);

	glfwMakeContextCurrent(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showTestWindow)
		{
			jed();
		}
		else if (showTest2Window)
		{
			dwa();
		}
		else if (showTest3Window)
		{
			trzy();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}