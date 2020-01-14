#define IMGUI_DEFINE_MATH_OPERATORS

#include "Custom.hpp"

namespace
{

inline float roundToNearest(float value, float interval)
{
    if (value >= 0)
    {
        float rem = fmod(value, interval);
        value = rem > interval * 0.5f ? value + interval - rem : value - rem;
    }
    else
    {
        value = -value;
        float rem = fmod(value, interval);
        value = rem > interval * 0.5f ? value + interval - rem : value - rem;
        value = -value;
    }
    return value;
}

} // namespace

namespace ImGui
{

bool MiniSliderScalar(const char *label, ImGuiDataType data_type, void *p_data, const void *p_min, const void *p_max, bool top, float power)
{
    ImGuiWindow *window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const float h = (label_size.y + style.FramePadding.y * 2.0f);
    const ImVec2 topLeft = top ? window->DC.CursorPos : window->DC.CursorPos + ImVec2(0, 0.5f * h + style.ItemSpacing.y * 0.5f);
    const ImRect frame_bb(topLeft, topLeft + ImVec2(w, h * 0.5f - style.ItemSpacing.y * 0.5f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max);

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Tabbing or CTRL-clicking on Slider turns it into an input box
    const bool hovered = ItemHoverable(frame_bb, id);
    const bool focus_requested = FocusableItemRegister(window, id);
    const bool clicked = (hovered && g.IO.MouseClicked[0]);
    if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, "", power, ImGuiSliderFlags_None, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x)
        window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    return value_changed;
}

bool MiniSliderFloat(const char *label, float *v, float v_min, float v_max, bool top, float power)
{
    return MiniSliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, top, power);
}

void RenderGrid(ImRect bb, int nx, int ny, ImU32 gridColor, ImU32 bgColor, float thickness)
{
    ImDrawList *DrawList = GetWindowDrawList();
    float dx = bb.GetWidth() / nx;
    float dy = bb.GetHeight() / ny;
    DrawList->AddRectFilled(bb.Min, bb.Max, bgColor);
    for (int ix = 0; ix < nx + 1; ++ix)
    {
        ImVec2 t = bb.Min + ImVec2(ix * dx, 0);
        ImVec2 b = t;
        b.y += bb.GetHeight();
        DrawList->AddLine(t, b, gridColor, thickness);
    }
    for (int iy = 0; iy < ny + 1; ++iy)
    {
        ImVec2 l = bb.Min + ImVec2(0, iy * dy);
        ImVec2 r = l;
        l.x += bb.GetWidth();
        DrawList->AddLine(l, r, gridColor, thickness);
    }
}

void PlotSignal(const char *label, const tact::Signal &sig, std::vector<ImVec2> &points, ImVec4 color, float thickness, ImVec2 size, bool grid)
{
    ImGuiWindow *window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImGuiIO &IO = GetIO();
    ImDrawList *DrawList = GetWindowDrawList();
    size = ImGui::CalcItemSize(size, 200, 200);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(frame_bb, style.FramePadding.y);
    if (!ItemAdd(frame_bb, 0, &frame_bb))
        return;
    const bool hovered = ItemHoverable(frame_bb, id);
    if (grid)
        RenderGrid(frame_bb, 10, 8, GetColorU32(ImGuiCol_WindowBg), GetColorU32(ImGuiCol_FrameBg));
    else
        RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const ImVec2 start = {frame_bb.Min.x, frame_bb.GetCenter().y};
    const float dx = frame_bb.GetWidth() / points.size();
    const float ys = -frame_bb.GetHeight() * 0.5f * 0.95f;
    float len = sig.length();
    if (len > 0)
    {
        if (len == tact::INF)
        {
            // ImGui::Text("INF");
            len = 1.0f;
        }
        // else
        //     ImGui::Text("%0.3f s", len);
        float dt = len / points.size();
        float t = 0;
        float x = start.x;

        for (int i = 0; i < points.size(); ++i)
        {
            float s = ImClamp((float)sig.sample(t), -1.0f, 1.0f);
            float y = start.y + s * ys;
            points[i].x = x;
            points[i].y = y;
            t += dt;
            x += dx;
        }
    }
    DrawList->Flags &= ~ImDrawListFlags_AntiAliasedLines;
    DrawList->AddPolyline(&points[0], (int)points.size(), ImGui::ColorConvertFloat4ToU32(color), false, thickness);
    DrawList->Flags |= ImDrawListFlags_AntiAliasedLines;
}

bool Spatializer(const char *label, SpatializerTarget &target, tact::Curve rolloff, std::map<int, SpatializerNode> &nodes, float nodeRadius,
                 ImVec4 color, ImVec2 size, const char *dnd, int xdivs, int ydivs, bool snap)
{

    static ImU32 gridColor = GetColorU32(ImGuiCol_WindowBg);
    static ImU32 gridBg = GetColorU32(ImGuiCol_FrameBgHovered, 0.1f);

    bool changed = false;
    // skip render if SkipItems on
    ImGuiWindow *window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    // get ImGuiContext and Style
    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImGuiIO &IO = GetIO();
    ImDrawList *DrawList = GetWindowDrawList();

    size = CalcItemSize(size, 100, 100);
    // background
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return false;
    const bool hovered = ItemHoverable(inner_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    // render grid
    const float gridSize = ImMin(frame_bb.GetWidth(), frame_bb.GetHeight()) - 3 * nodeRadius;
    const ImVec2 offset = ImVec2(gridSize, gridSize) * 0.5f;
    const ImRect grid = ImRect(frame_bb.GetCenter() - offset, frame_bb.GetCenter() + offset);
    const ImVec2 grid_cntr = grid.GetCenter();
    const float grid_width = grid.GetWidth();
    const float grid_height = grid.GetHeight();
    RenderGrid(grid, xdivs - 1, ydivs - 1, gridColor, gridBg);
    if (xdivs == 1)
        DrawList->AddLine(ImVec2(grid_cntr.x, grid.Min.y), ImVec2(grid_cntr.x, grid.Max.y), gridColor, 1.0f);
    if (ydivs == 1)
        DrawList->AddLine(ImVec2(grid.Min.x, grid_cntr.y), ImVec2(grid.Max.x, grid_cntr.y), gridColor, 1.0f);

    // transform lambdas
    auto toPx = [&](const ImVec2 &nm) -> ImVec2 {
        float pxX = xdivs > 1 ? grid.Min.x + nm.x * grid_width : grid_cntr.x;
        float pxY = ydivs > 1 ? grid.Max.y - nm.y * grid_height : grid_cntr.y;
        return ImVec2(pxX, pxY);
    };
    auto toNm = [&](const ImVec2 px) -> ImVec2 {
        float nmX = xdivs > 1 ? (px.x - grid.Min.x) / grid_width : 0;
        float nmY = ydivs > 1 ? -(px.y - grid.Max.y) / grid_height : 0;
        return ImVec2(nmX, nmY);
    };
    auto snapNode = [&](SpatializerNode &node) {
        if (xdivs > 1)
            node.pos.x = roundToNearest(node.pos.x, 1.0f / (xdivs - 1));
        else
            node.pos.x = 0;
        if (ydivs > 1)
            node.pos.y = roundToNearest(node.pos.y, 1.0f / (ydivs - 1));
        else
            node.pos.y = 0;
    };

    // node ineractions
    const ImVec2 mousePos = IO.MousePos;
    const float r2 = nodeRadius * nodeRadius;
    int toDelete = -1;
    bool firstGrab = true;
    for (auto rit = nodes.rbegin(); rit != nodes.rend(); ++rit)
    {
        SpatializerNode &node = rit->second;
        ImVec2 posPx = toPx(node.pos);
        ImVec2 v = mousePos - posPx;
        float d = ImLengthSqr(v);
        bool inside = d < r2;
        if (inside && IO.MouseClicked[0] && firstGrab)
        {
            node.held = true;
            firstGrab = false;
        }
        if (inside && IO.MouseDoubleClicked[0] && toDelete == -1)
            toDelete = node.index;
        if (node.held && IO.MouseReleased[0])
            node.held = false;
        // drag
        if (node.held)
        {
            if (IO.MouseDelta.x != 0 || IO.MouseDelta.y != 0) {
                // changed = true;
                posPx += IO.MouseDelta;
                posPx.x = ImClamp(posPx.x, xdivs == 1 ? grid_cntr.x : grid.Min.x, xdivs == 1 ? grid_cntr.x : grid.Max.x);
                posPx.y = ImClamp(posPx.y, ydivs == 1 ? grid_cntr.y : grid.Min.y, ydivs == 1 ? grid_cntr.y : grid.Max.y);
            }
        }
        node.pos = toNm(posPx);
        if (snap && !node.held)
            snapNode(node);
    }
    // render nodes
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        SpatializerNode &node = it->second;
        float dtarget = std::sqrt(ImLengthSqr(node.pos - target.pos));
        float ttarget = ImClamp(dtarget / target.radius, 0.0f, 1.0f);
        ttarget = 1.0f - ttarget;
        ttarget = (float)rolloff(ttarget);
        ImVec4 heldColor = ImLerp(style.Colors[ImGuiCol_ButtonActive], color, ttarget);
        ImVec4 normColor = ImLerp(style.Colors[ImGuiCol_ButtonHovered], color, ttarget);
        auto posPx = toPx(node.pos);
        DrawList->AddCircleFilled(posPx, nodeRadius, node.held ? ColorConvertFloat4ToU32(heldColor) : ColorConvertFloat4ToU32(normColor), 25);
        std::string id_label = std::to_string(node.index);
        const ImVec2 label_size = CalcTextSize(id_label.c_str(), NULL, true);
        DrawList->AddText(posPx - label_size / 2, GetColorU32(ImGuiCol_Text), id_label.c_str());
    }

    // delete
    if (toDelete != -1)
    {
        nodes.erase(toDelete);
        changed = true;
    }
    // render target
    ImVec2 targetPosPx = toPx(target.pos);
    if (IO.MouseDown[1] && hovered)
    {
        targetPosPx = mousePos;
    }
    if (xdivs == 1)
        targetPosPx.x = grid_cntr.x;
    if (ydivs == 1)
        targetPosPx.y = grid_cntr.y;

    float r = target.radius * grid.GetWidth();
    if (hovered)
    {
        r += IO.MouseWheel;
        r = ImClamp(r, 1.0f, FLT_MAX);
    }

    DrawList->PushClipRect(grid.Min, grid.Max, true);
    if (xdivs > 1 && ydivs > 1) {
        int rings = 20;
        color.w = 0.2f;
        DrawList->AddCircleFilled(targetPosPx, 0.5f * r / rings, ColorConvertFloat4ToU32(color), 25);
        for (int i = 0; i < rings+1; ++i) {
            float t = (i + 1.0f) / rings;
            color.w = rolloff(1-t) * 0.2f;
            DrawList->AddCircle(targetPosPx, t * r, ColorConvertFloat4ToU32(color), 25, r / rings);
        }
    }
    else
    {
        ImVec2 boxMin = targetPosPx - ImVec2(r, r);
        ImVec2 boxMax = targetPosPx + ImVec2(r, r);
        if (xdivs == 1)
        {
            boxMin.x = grid.Min.x;
            boxMax.x = grid.Max.x;
        }
        if (ydivs == 1)
        {
            boxMin.y = grid.Min.y;
            boxMax.y = grid.Max.y;
        }
        DrawList->AddRectFilled(boxMin, boxMax, ColorConvertFloat4ToU32(color));
    }
    DrawList->PopClipRect();
    target.pos = toNm(targetPosPx);
    target.radius = r / grid.GetWidth();
    // render position label
    if (grid.Contains(mousePos))
    {
        char posText[16];
        ImVec2 mouseNm = toNm(mousePos);
        sprintf(posText, "%.2f,%.2f", mouseNm.x, mouseNm.y);
        ImVec2 posTextSize = CalcTextSize(posText);
        RenderText(grid.Max - posTextSize - style.FramePadding, posText);
    }
    // drag and drop
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(dnd))
        {
            auto dropped = *(int *)payload->Data;
            SpatializerNode node;
            node.index = dropped;
            node.pos = ImClamp(toNm(mousePos), ImVec2(0, 0), ImVec2(1, 1));
            node.held = false;
            if (snap)
                snapNode(node);
            nodes[node.index] = node;
            changed = true;
        }
        ImGui::EndDragDropTarget();
    }
    return changed;
}

} // namespace ImGui