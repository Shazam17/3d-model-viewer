#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const float width = 1280;
const float height = 720;

float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


double xpos;
double ypos;

const float Pi = 3.14;

float theta = Pi/4;
float fi = Pi/4;
float radius = 2;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool isPressed = false;

glm::vec2 posWhenPressed;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		isPressed = true;

		posWhenPressed = glm::vec2(xpos, ypos);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isPressed = false;
	}
		
}

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	xpos = x;
	ypos = y;

	if (isPressed) {
		float deltaX = xpos - posWhenPressed.x;
		float deltaY = ypos - posWhenPressed.y;
		deltaX = -deltaX;


		glm::vec2 pos(deltaX, deltaY);
		float len = pos.length();
		pos.x = pos.x / len;
		pos.y = pos.y / len;

		std::cout << theta << '|' << fi << std::endl;

		theta += pos.x * 0.0001;
		fi += pos.y * 0.0001;

		if (fi < 0) {
			fi = 0.01;
		}


		if (fi > 3.14) {
			fi = 3.13999;
		}
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	radius -= yoffset * 0.1;
}
class Vertex {
	glm::vec3 pos;
	glm::vec3 norm;

public:
	Vertex(float arr[3]) {
		pos.x = arr[0];
		pos.y = arr[1];
		pos.z = arr[2];
	}

};

class Model {

	unsigned vbo, vao, ib;
	unsigned count;
public:
	Model(const std::vector<Vertex>& v, const std::vector<unsigned>&i) {
		

		count = i.size();
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * v.size(), &v[0], GL_STATIC_DRAW);

		glGenBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * i.size(), &i[0], GL_STATIC_DRAW);



		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	}


	void draw(const Shader& shad) {
		shad.bind();
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	}

};

Model loadObj(const std::string& path) {

	std::vector<Vertex> v;
	std::vector<unsigned> i;

	std::ifstream file;
	try {
		file.open(path);
	}
	catch (const std::exception & e) {
		std::cout << "failed to load model" << std::endl;
	}
	
	std::string buff;
	while (std::getline(file, buff)) {
		std::stringstream ss;
		ss << buff;
		std::string type;
		ss >> type;
		

		

		if (type.compare("v") == 0) {
			float cord[3];
			for (int i = 0; i < 3; i++) {
				ss >> cord[i];
			}

			v.push_back(Vertex(cord));
		}
		if (type.compare("f") == 0) {
			std::stringstream indicesStream;
			indicesStream << ss.rdbuf();
			
			std::vector<unsigned> t;
			std::string stringBuff;
			unsigned unBuff;
			while (indicesStream >> stringBuff) {
				auto inStream = std::stringstream(stringBuff);
				while (inStream >> unBuff) {
					t.push_back(unBuff);
					char del;
					inStream >> del;

				}
			}

			if (t.size() == 12) {
				i.push_back(t[0]);
				i.push_back(t[3]);
				i.push_back(t[6]);

				i.push_back(t[0]);
				i.push_back(t[6]);
				i.push_back(t[9]);

			}
			if (t.size() == 9) {
				i.push_back(t[0]);
				i.push_back(t[3]);
				i.push_back(t[6]);

			

			}
			
		}
	}
	for (auto& index : i) {
		index--;
	}

	return Model(v,i);
}


int main(void)
{
	GLFWwindow* window;

	
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(width, height, "arcBallTest", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//SETTING UP BUFFERS


	auto md = loadObj("tor.obj");


	unsigned vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);



	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	Shader shad("basic.shader");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
	glm::mat4 model = glm::scale(glm::mat4(1.0f),glm::vec3(0.8f));
	glm::mat4 view = glm::lookAt(glm::vec3(1), glm::vec3(0), glm::vec3(0, 1, 0));

	shad.setUniform4m("u_proj", proj);
	shad.setUniform4m("u_model", model);
	shad.setUniform4m("u_view", view);



	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	std::string path = "tor.obj";

	bool isRendering = true;
	while (!glfwWindowShouldClose(window))
	{
		std::string lastPath = path;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::InputText("path", &path[0],64);
		if (path.compare(lastPath) != 0) {
			isRendering = false;
		}

		if (ImGui::Button("load model")) {
			md = loadObj(path);
			isRendering = true;
		}


		float x = -radius * cos(theta) * sin(fi);
	
		float y = radius * cos(fi);
		float z = radius * sin(theta) * sin(fi);
		//view = glm::lookAt(glm::vec3(x,z,y), glm::vec3(0), glm::vec3(0, 1, 0));
		view = glm::lookAt(glm::vec3(x,y,z), glm::vec3(0), glm::vec3(0, 1, 0));
		shad.setUniform4m("u_view", view);

		shad.bind();
		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		if (isRendering) {
			md.draw(shad);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
	glfwTerminate();
	glfwTerminate();
	return 0;
}