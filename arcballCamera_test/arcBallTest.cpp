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


const float Pi = 3.14;

float radius = 2;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	radius -= yoffset * 0.1f;
	
}
class Vertex {
	glm::vec3 pos;

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		

	}

	static Model loadObj(const std::string& path) {

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
						t.push_back(--unBuff);
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


		return Model(v, i);
	}

	void draw(const Shader& shad) {
		shad.bind();
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	}

};

class Application {
private:
	GLFWwindow* window;
	float aspect;
	float width;
	float height;
	friend class OrbitCamera;

public:
	Application(float w, float h, const std::string& windowName) : width(w), height(h), aspect(w/h) {

		if (!glfwInit())
			throw std::exception("glfw init failed");

		window = glfwCreateWindow(w, h, windowName.c_str(), NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			throw std::exception("glfw window init failed");
		}


		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::exception("glad init failed");
		}

		glEnable(GL_DEPTH_TEST);
		glfwSetScrollCallback(window, scroll_callback);


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
	}

	bool isRunning() {
		return !glfwWindowShouldClose(window);
	}


	void clear(const glm::vec4& clearColor = glm::vec4(0.0f)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void swapBuffers() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	Application() {
		//Shutdown
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();
		glfwTerminate();
		glfwTerminate();
	}
};

class OrbitCamera {

	float theta;
	float fi;
	float fov;
	

	float x;
	float y;

	bool pressed = false;
	glm::vec2 posLastPressed;
	glm::mat4 view;

public:

	OrbitCamera(float fov = 45.0f, float th = Pi / 4, float f = Pi / 4) : theta(th), fi(f), fov(glm::radians(fov)) {

	}

	void processMouseKes(const Application& app) {
		if (glfwGetMouseButton(app.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			pressed = true;
			posLastPressed = glm::vec2(x, y);
		}

		if (glfwGetMouseButton(app.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			pressed = false;
		}
	}

	void processMouseMove(const Application& app) {
		double xpos, ypos;
		glfwGetCursorPos(app.window, &xpos, &ypos);

		x = xpos;
		y = ypos;

		if (pressed) {
			//deltaX = -deltaX;
			glm::vec2 pos = glm::vec2(xpos, ypos) - posLastPressed;
			pos /= pos.length();


			theta -= pos.x * 0.01;
			fi    += pos.y * 0.01;

			if (fi < 0) {
				fi = 0.01;
			}


			if (fi > 3.14) {
				fi = 3.13999;
			}
		}

	}

	glm::mat4 getViewMat() {
		
		float X = -radius * cos(theta) * sin(fi);
		float Y = radius * cos(fi);
		float Z = radius * sin(theta) * sin(fi);
		view = glm::lookAt(glm::vec3(X, Y, Z), glm::vec3(0), glm::vec3(0, 1, 0));
		return view;
	}

	glm::mat4 getProjectionMat(const Application& app) {
		return glm::perspective(fov, app.aspect, 0.1f, 100.0f);
	}

};


int main(void)
{
	Application app(1280, 720, "3d model viewer");
	OrbitCamera camera;

	auto md = Model::loadObj("tor.obj");
	std::string path = "tor.obj";

	Shader shad("basic.shader");
	glm::mat4 proj = camera.getProjectionMat(app);
	shad.setUniform4m("u_proj", proj);


	bool isRendering = true;
	while (app.isRunning())
	{
		app.clear();
		camera.processMouseMove(app);
		camera.processMouseKes(app);


		//check if need to load new 3d model
		std::string lastPath = path;
		ImGui::InputText("path", &path[0],64);
		if (path.compare(lastPath) != 0) {
			isRendering = false;
		}
		if (ImGui::Button("load model")) {
			md = Model::loadObj(path);
			isRendering = true;
		}

		glm::mat4 view = camera.getViewMat();
		shad.setUniform4m("u_view", view);



		if (isRendering) {
			shad.bind();
			md.draw(shad);
		}

		app.swapBuffers();
	}
	
	return 0;
}


