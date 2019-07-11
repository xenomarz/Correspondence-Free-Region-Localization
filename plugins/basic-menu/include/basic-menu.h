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


using namespace std;
using namespace Eigen;

// Expose an enumeration type
enum Orientation { Two_views = 0, Left_view, Right_view };
enum View {Left = 0, Right};
enum MouseMode { NONE=0, FACE_SELECT, VERTEX_SELECT , CLEAR};
enum Parametrization { HARMONIC =0, LSCM, ARAP};

static Orientation view;


namespace rds
{
	namespace plugins
	{
		class BasicMenu : public igl::opengl::glfw::imgui::ImGuiMenu
		{
		protected:
			std::map<unsigned int, std::string> data_id_to_name;
			Eigen::Vector3d onMouse_triangle_color;
			Eigen::Vector3d selected_faces_color;
			Eigen::Vector3d selected_vertices_color;
			Eigen::Vector3d model_color;
			Eigen::MatrixXd colors_per_face;
			std::set<int> selected_faces;
			std::set<int> selected_vertices;
			int ShowModelIndex;
			bool test_bool;
			MouseMode mouse_mode;
			Parametrization param_type;
		public:
			BasicMenu();
			IGL_INLINE virtual void draw_viewer_menu() override;
			void set_name_mapping(unsigned int data_id, std::string name);
			int CurrmodelID(View LR);
			int pick_face(Eigen::MatrixXd& V, Eigen::MatrixXi& F, View LR);
			int pick_vertex(Eigen::MatrixXd& V, Eigen::MatrixXi& F, View LR);
			void follow_and_mark_selected_faces();
			void Update_view();
			char* getModelNames();
			bool mouse_down(int button, int modifier);
			void compute_ARAP_param(int model_index);
			void compute_harmonic_param(int model_index);
			void compute_lscm_param(int model_index);
		};
	}
}

#endif
