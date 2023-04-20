#include "ConsolePanel.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <mutex>

#include "imgui/imgui.h"

void ConsolePanel::Render()
{
    if (ImGui::Begin("Console"))
    {

        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));  // Tighten spacing

        {
            LogManager* pLogManager = LogManager::GetSingletonPtr();
            if (pLogManager)
            {
                pLogManager->GetLog(mLogs);
            }
        }

        for (const auto& logItem : mLogs)
        {
            uint32_t colorCode = logItem.first;
            float r = ((colorCode & 0xFF0000) >> 16) / 255.f;
            float g = ((colorCode & 0x00FF00) >> 8) / 255.f;
            float b = ((colorCode & 0x0000FF) >> 0) / 255.f;
            ImVec4 color = ImVec4(r, g, b, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(logItem.second.c_str());
            ImGui::PopStyleColor();
        }

        if (mScrollToBottom || (mAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        {
            ImGui::SetScrollHereY(1.0f);
        }
        mScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        if (ImGui::SmallButton("Clear"))
        {
            LogManager* pLogManager = LogManager::GetSingletonPtr();
            if (pLogManager)
            {
                pLogManager->ClearLog();
            }
        }
        ImGui::SameLine();

        ImGui::Separator();

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
    }
    ImGui::End();
}