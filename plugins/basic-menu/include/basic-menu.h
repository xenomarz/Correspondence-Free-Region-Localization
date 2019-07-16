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

#define RED_COLOR Eigen::Vector3f(1, 0, 0)
#define BLUE_COLOR Eigen::Vector3f(0, 0, 1)
#define GREEN_COLOR Eigen::Vector3f(0, 1, 0)
#define GOLD_COLOR Eigen::Vector3f(1, 215.0f / 255.0f, 0)
#define MODEL2_PATH "..\\..\\..\\models\\camel_head.off"
#define MODEL1_PATH "..\\..\\..\\models\\cube.off"




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
			enum View { Horizontal = 0, Vertical, Core_1, Core_2 };
			enum MouseMode { NONE = 0, FACE_SELECT, VERTEX_SELECT, CLEAR };
			enum Parametrization { HARMONIC = 0, LSCM, ARAP };

			//Basic (necessary) parameteres
			int left_view_id, right_view_id, ShowModelIndex;
			View view;
			MouseMode mouse_mode;
			Parametrization param_type;
			Vector3f onMouse_triangle_color, selected_faces_color, selected_vertices_color, model_color;
			MatrixXd colors_per_face;
			set<int> selected_faces, selected_vertices;
			float core_percentage_size;
			igl::opengl::glfw::imgui::ImGuiMenu menu;

			//Solver Parameters
			bool SolverMode;
			
			//Parametrization Parameters
			float Lambda, Delta, Integer_Weight, Integer_Spacing, Seamless_Weight, Position_Weight;


		protected:
			//Basic (necessary) parameteres
			std::map<unsigned int, string> data_id_to_name;
			
		public:
			//Constructor & initialization
			BasicMenu();
			IGL_INLINE virtual void init(igl::opengl::glfw::Viewer *_viewer) override;

			//Draw menu methods
			IGL_INLINE virtual void draw_viewer_menu() override;
			void Draw_menu_for_cores();
			void Draw_menu_for_models();
			void Draw_menu_for_Parametrization();
			void Draw_menu_for_Solver();

			//Pick faces & vertices and highlight them
			int pick_face(Eigen::MatrixXd& V, Eigen::MatrixXi& F, View LR);
			int pick_vertex(Eigen::MatrixXd& V, Eigen::MatrixXi& F, View LR);
			void follow_and_mark_selected_faces();

			//Name's methods
			void set_name_mapping(unsigned int data_id, string name);
			char* getModelNames();
			string filename(const string& str);

			//Basic Methods
			int LeftModelID();
			int RightModelID();
			void Update_view();
			
			//Events methods
			bool mouse_down(int button, int modifier);
			void post_resize(int w, int h);

			//Parametrizations
			void compute_ARAP_param(int model_index);
			void compute_harmonic_param(int model_index);
			void compute_lscm_param(int model_index);
		};
	}
}

#endif
