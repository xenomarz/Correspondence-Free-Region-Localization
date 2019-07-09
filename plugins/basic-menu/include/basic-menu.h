#ifndef RDS_PLUGINS_BASIC_MENU_H
#define RDS_PLUGINS_BASIC_MENU_H

#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/igl_inline.h>
#include <map>

// Expose an enumeration type
enum Orientation { Two_views = 0, Left_view, Right_view };
static Orientation view;

namespace rds
{
	namespace plugins
	{
		class BasicMenu : public igl::opengl::glfw::imgui::ImGuiMenu
		{
		protected:
			std::map<unsigned int, std::string> data_id_to_name;
			

		public:
			int show_models[2];
			bool test_bool;
			BasicMenu();
			IGL_INLINE virtual void draw_viewer_menu() override;
			void set_name_mapping(unsigned int data_id, std::string name);
			
			char* getModelNames();
		};
	}
}

#endif
