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
#include "../../libs/optimization_lib/include/objective_functions/objective_symmetric_dirichlet.h"
#include "../../libs/optimization_lib/include/objective_functions/penalty_positional_constraints.h"
#include <atomic>


#define RED_COLOR Eigen::Vector3f(1, 0, 0)
#define BLUE_COLOR Eigen::Vector3f(0, 0, 1)
#define GREEN_COLOR Eigen::Vector3f(0, 1, 0)
#define GOLD_COLOR Eigen::Vector3f(1, 215.0f / 255.0f, 0)
#define GREY_COLOR Eigen::Vector3f(0.75, 0.75, 0.75)
//#define MODEL1_PATH "..\\..\\..\\models\\cactus.off"
//#define MODEL1_PATH "..\\..\\..\\models\\camel_head.off"
#define MODEL1_PATH "..\\..\\..\\models\\cube.off"
//#define MODEL1_PATH "..\\..\\..\\models\\woody-hi.off"




using namespace std;
using namespace Eigen;


namespace rds
{
	namespace plugins
	{
		class BasicMenu : public igl::opengl::glfw::imgui::ImGuiMenu
		{
		private:
			// Expose an enumeration type
			enum View { Horizontal = 0, Vertical, InputOnly, OutputOnly };
			enum MouseMode { NONE = 0, FACE_SELECT, VERTEX_SELECT, CLEAR };
			enum Parametrization { HARMONIC = 0, LSCM, ARAP };

			//Basic (necessary) parameteres
			int input_view_id, output_view_id, ShowModelIndex;
			View view;
			MouseMode mouse_mode;
			Parametrization param_type;
			Vector3f Highlighted_face_color, Fixed_face_color, Fixed_vertex_color;
			Vector3f model_color, Dragged_face_color, Dragged_vertex_color, Vertex_Energy_color;
			MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
			set<int> selected_faces, selected_vertices;
			float core_percentage_size, texture_size;
			bool IsTranslate;
			int Translate_Index, Model_Translate_ID, Core_Translate_ID, down_mouse_x, down_mouse_y;
			igl::opengl::glfw::imgui::ImGuiMenu menu;

			std::vector<int> *HandlesInd; //pointer to indices in constraitPositional
			MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional

			//Solver Button Parameters
			bool solver_on;

			//Parametrization Parameters
			float Lambda, Delta, Integer_Weight, Integer_Spacing, Seamless_Weight, Position_Weight;

			// Solver thread
			thread solver_thread;
			unique_ptr<Newton> solver;
			shared_ptr<TotalObjective> totalObjective;

		protected:
			//Basic (necessary) parameteres
			std::map<unsigned int, string> data_id_to_name;

		public:
			//Constructor & initialization
			BasicMenu();
			~BasicMenu();

			// callbacks
			IGL_INLINE virtual void draw_viewer_menu() override;
			IGL_INLINE virtual void init(igl::opengl::glfw::Viewer *_viewer) override;
			IGL_INLINE virtual void post_resize(int w, int h) override;
			IGL_INLINE virtual bool mouse_move(int mouse_x, int mouse_y) override;
			IGL_INLINE virtual bool mouse_down(int button, int modifier) override;
			IGL_INLINE virtual bool mouse_up(int button, int modifier) override;
			IGL_INLINE virtual bool pre_draw() override;
			IGL_INLINE virtual void shutdown() override;
			
			//Draw menu methods
			void Draw_menu_for_cores();
			void Draw_menu_for_models();
			void Draw_menu_for_Parametrization();
			void Draw_menu_for_Solver();

			//Pick faces & vertices and highlight them
			int pick_face(Eigen::MatrixXd& V, Eigen::MatrixXi& F, View LR);
			int pick_vertex(Eigen::MatrixXd& V, Eigen::MatrixXi& F, View LR);
			void follow_and_mark_selected_faces();
			RowVector3d get_face_avg();
			Vector3f computeTranslation(int mouse_x,int from_x,int mouse_y,int from_y,RowVector3d pt3D);
			void UpdateHandles();

			//Name's methods
			void set_name_mapping(unsigned int data_id, string name);
			char* getModelNames();
			string filename(const string& str);

			//Basic Methods
			int InputModelID();
			int OutputModelID();
			void Update_view();
			void update_mesh();

			//Parametrizations
			void compute_ARAP_param(int model_index);
			void compute_harmonic_param(int model_index);
			void compute_lscm_param(int model_index);

			//Start/Stop the solver Thread
			void start_solver_thread();
			void stop_solver_thread();
			void initializeSolver();

			//FD check
			void checkGradients();
			void checkHessians();
		};
	}
}

#endif