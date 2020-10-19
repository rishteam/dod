#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>

#include <utility>

namespace ImGui {

/**
 * @brief Get Window Content Size
 * @return [Width, Height]
 *
 * @details
 * < Width >
 * +-------+
 * +-------+^
 * |Content| Height
 * +-------+v
 */
ImVec2 GetWindowContentSize();

/**
* @brief
* @return
*
* @details
* A-------x
* |       |
* x-------B
* @return [Upper left(A), Bottom right(B)]
*/
std::pair<ImVec2, ImVec2> GetWindowContentPoints();

/**
 * @brief Get Mouse position relative to the current window
 */
ImVec2 GetMousePosRelatedToWindow();

ImVec2 GetMousePosRelatedToWindowNormalize();
ImVec2 GetMousePosRelatedToWindowNormalizeCenter();

void BeginDockspace(const char *name);
void EndDockspace();

void HelpMarker(const char* desc);

//
// Support STL containers
bool Combo(const char *label, int *current_item, const std::vector<std::string> &v, int popup_max_height_in_items = -1);


bool ListBox(const char* label, int* current_item, const std::vector<std::string> &v, int height_in_items=-1);



} // end of namespace ImGui

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs);
ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs);
