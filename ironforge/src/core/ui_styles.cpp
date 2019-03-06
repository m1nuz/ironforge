#include "game_detail.hpp"

namespace game {

    namespace detail {

//        auto to_color(const std::string& color_text) -> ui::color_t  {
//            if (color_text.empty())
//                return 0;

//            if (color_text[0] == '#')
//            {
//                unsigned int color = 0;
//                std::stringstream ss;
//                ss << std::hex << &color_text[1];
//                ss >> color;

//                return color;
//            }

//            return std::stoi(color_text);
//        }

//        auto read_ui_styles( assets::instance_t& asset, json ui_json) -> std::unordered_map<std::string, imui::box_style> {
//            using namespace std;

//            std::unordered_map<std::string, imui::box_style> styles;

//            if (ui_json.find("theme") != ui_json.end()) {
//                const auto theme_filename = ui_json["theme"].get<string>();

//                const auto theme_text = assets::get_text(asset, theme_filename);

//                if (theme_text) {
//                    if (!theme_text.value().empty()) {
//                        auto j = json::parse(theme_text.value());

//                        for (auto it = j.begin(); it != j.end(); ++it)
//                        {
//                            const auto style_name = it.key();
//                            const auto style_properties = it.value();
//                            std::cout << style_name << " | " << style_properties << "\n";

//                            imui::box_style style;

//                            if (style_properties.find("width") != style_properties.end())
//                                style.width = style_properties["width"].get<float>();

//                            if (style_properties.find("height") != style_properties.end())
//                                style.height = style_properties["height"].get<float>();

//                            if (style_properties.find("border-width") != style_properties.end())
//                                style.border_width = style_properties["border-width"].get<float>();

//                            if (style_properties.find("border-color") != style_properties.end())
//                            {
//                                const auto color = to_color(style_properties["border-color"].get<string>());
//                                style.border_color = std::array<ui::color_t, 4>{color, color, color, color};
//                            }

//                            if (style_properties.find("background-color") != style_properties.end())
//                                style.background_color = to_color(style_properties["background-color"].get<string>());

//                            if (style_properties.find("foreground-color") != style_properties.end())
//                                style.foreground_color = to_color(style_properties["foreground-color"].get<string>());

//                            if (style_properties.find("active-color") != style_properties.end())
//                                style.active_color = to_color(style_properties["active-color"].get<string>());

//                            if (style_properties.find("focused-color") != style_properties.end())
//                                style.focused_color = to_color(style_properties["focused-color"].get<string>());

//                            if (style_properties.find("text-color") != style_properties.end())
//                                style.text_color = to_color(style_properties["text-color"].get<string>());

//                            styles.emplace(style_name, style);
//                        }
//                    }
//                }
//            }

//            return styles;
//        }

    } // namespace detail

} // namespace game
