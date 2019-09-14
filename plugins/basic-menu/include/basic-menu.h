#ifndef RDS_PLUGINS_BASIC_MENU_H
#define RDS_PLUGINS_BASIC_MENU_H

#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/igl_inline.h>
#include <map>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <igl/project.h>
#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_fonts_droid_sans.h>
#include <GLFW/glfw3.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/ViewerPlugin.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <igl/unproject_in_mesh.h>
#include <igl/Hit.h>
#include <igl/rotate_by_quat.h>
#include <memory>
#include <igl/boundary_loop.h>
#include <igl/readOFF.h>
#include <igl/lscm.h>
#include <igl/harmonic.h>
#include <igl/map_vertices_to_circle.h>
#include <igl/arap.h>
#include <igl/file_dialog_open.h>
#include <igl/unproject.h>
#include "../../libs/optimization_lib/include/solvers/newton_solver.h"
#include "../../libs/optimization_lib/include/solvers/gradient_descent_solver.h"
#include "../../libs/optimization_lib/include/objective_functions/objective_symmetric_dirichlet.h"
#include "../../libs/optimization_lib/include/objective_functions/objective_area_preserving.h"
#include "../../libs/optimization_lib/include/objective_functions/penalty_positional_constraints.h"
#include <atomic>

#define RED_COLOR Vector3f(1, 0, 0)
#define BLUE_COLOR Vector3f(0, 0, 1)
#define GREEN_COLOR Vector3f(0, 1, 0)
#define GOLD_COLOR Vector3f(1, 215.0f / 255.0f, 0)
#define GREY_COLOR Vector3f(0.75, 0.75, 0.75)
#define WHITE_COLOR Vector3f(1, 1, 1)

using namespace std;
using namespace Eigen;
using namespace igl;
using namespace opengl;
using namespace glfw;
using namespace imgui;

class BasicMenu : public ImGuiMenu
{
private:
	// Expose an enumeration type
	enum View { Horizontal = 0, Vertical, InputOnly, OutputOnly };
	enum MouseMode { NONE = 0, FACE_SELECT, VERTEX_SELECT, CLEAR };
	enum Parametrization { RANDOM = 0, HARMONIC, LSCM, ARAP, None };
	enum Distortion { NO_DISTORTION, AREA_DISTORTION, LENGTH_DISTORTION, ANGLE_DISTORTION, TOTAL_DISTORTION };

	//Basic (necessary) parameteres
	int input_view_id, output_view_id;
	View view;
	MouseMode mouse_mode;
	Parametrization param_type;
	Vector3f Highlighted_face_color, Fixed_face_color, Fixed_vertex_color;
	bool Highlighted_face;
	Vector3f model_color, Dragged_face_color, Dragged_vertex_color, Vertex_Energy_color, text_color;
	MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
	set<int> selected_faces, selected_vertices;
	float core_percentage_size, texture_size;
	bool IsTranslate;
	unsigned int ShowModelIndex;
	int Translate_Index, Model_Translate_ID, Core_Translate_ID, down_mouse_x, down_mouse_y;
	ImGuiMenu menu;

	vector<int> *HandlesInd; //pointer to indices in constraitPositional
	MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional

	//Solver Button Parameters
	bool solver_on, solverInitialized, show_text;
	Distortion distortion_type;
	float Max_Distortion;
	
	// Solver thread
	thread solver_thread;
	unique_ptr<Newton> solver;
	shared_ptr<TotalObjective> totalObjective;
	shared_ptr<PenaltyPositionalConstraints> constraintsPositional;

protected:
	//Basic (necessary) parameteres
	map<unsigned int, string> data_id_to_name;

public:
	//Constructor & initialization
	BasicMenu();
	~BasicMenu(){}

	// callbacks
	IGL_INLINE virtual void draw_viewer_menu() override;
	IGL_INLINE virtual void init(Viewer *_viewer) override;
	IGL_INLINE virtual void post_resize(int w, int h) override;
	IGL_INLINE virtual bool mouse_move(int mouse_x, int mouse_y) override;
	IGL_INLINE virtual bool mouse_down(int button, int modifier) override;
	IGL_INLINE virtual bool mouse_up(int button, int modifier) override;
	IGL_INLINE virtual bool pre_draw() override;
	IGL_INLINE virtual void shutdown() override;
	IGL_INLINE virtual bool key_pressed(unsigned int key, int modifiers) override;
			
	//Draw menu methods
	void Draw_menu_for_cores();
	void Draw_menu_for_models();
	void Draw_menu_for_Solver();
	void Draw_menu_for_colors();
	void Draw_menu_for_text_results();

	//Pick faces & vertices and highlight them
	int pick_face(MatrixXd& V, MatrixXi& F, View LR);
	int pick_vertex(MatrixXd& V, MatrixXi& F, View LR);
	void follow_and_mark_selected_faces();
	RowVector3d get_face_avg();
	Vector3f computeTranslation(int mouse_x,int from_x,int mouse_y,int from_y,RowVector3d pt3D);
	void UpdateHandles();
	void UpdateEnergyColors();

	//Name's methods
	void set_name_mapping(unsigned int data_id, string name);
	char* getModelNames();
	string filename(const string& str);
	bool IsMesh2D();

	//Basic Methods
	int InputModelID();
	int OutputModelID();
	void Update_view();
	void update_mesh();

	//Parametrizations
	MatrixXd compute_ARAP_param();
	MatrixXd compute_harmonic_param();
	MatrixXd compute_lscm_param();
	MatrixXd ComputeSoup2DRandom();
	void FixFlippedFaces(MatrixXi& Fs, MatrixXd& Vs);
	void update_texture(MatrixXd& V_uv);

	//Start/Stop the solver Thread
	void start_solver_thread();
	void stop_solver_thread();
	void initializeSolver();

	//FD check
	void checkGradients();
	void checkHessians();
};

#endif