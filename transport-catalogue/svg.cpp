#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<< (std::ostream& out, const Color& data) {
        std::visit(OstreamColorPrinter{out}, data);
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const StrokeLineCap& data) {
        std::string output;
        switch (data) {
            case StrokeLineCap::BUTT:
                output = "butt";
                break;
            case StrokeLineCap::ROUND:
                output = "round";
                break;
            case StrokeLineCap::SQUARE:
                output = "square";
                break;
        }
        return out << output;
    }

    std::ostream& operator<< (std::ostream& out, const StrokeLineJoin& data){
        std::string output;
        switch (data) {
            case StrokeLineJoin::ARCS:
                output = "arcs";
                break;
            case StrokeLineJoin::BEVEL:
                output = "bevel";
                break;
            case StrokeLineJoin::MITER:
                output = "miter";
                break;
            case StrokeLineJoin::MITER_CLIP:
                output = "miter-clip";
                break;
            case StrokeLineJoin::ROUND:
                output = "round";
                break;
        }
        return out << output;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- PolyLine ------------------

    Polyline& Polyline::AddPoint(Point point){
        if(point.x == 0 && point.y == 0){
            return *this;
        }
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        if(points_.empty()){
            out << "<polyline points=\"\" />"sv;
        } else {
            out << "<polyline points=\""sv;
            for(int i = 0; i < static_cast<int>(points_.size()-1); ++i){
                out << points_[i].x << ","sv << points_[i].y << " "sv;
            }
            out << points_[points_.size()-1].x << ","sv << points_[points_.size()-1].y  <<"\"";
            RenderAttrs(out);
            out << "/>"sv;
        }
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos){
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset){
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size){
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family){
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight){
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data){
        data_ = Shielding(std::move(data));
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const{
        auto& out = context.out;
        out << "<text";
        RenderAttrs(out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y;
        out << "\" font-size=\""sv << size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        out << ">"sv << data_ << "</text>"sv;
    }

    std::string Text::Shielding(std::string str) {
        auto pos_amp = str.find('&');
        while(pos_amp != std::string::npos) {
            str.replace(pos_amp, 1, "&amp;"s);
            pos_amp = str.find('&', pos_amp + 5);
        }
        auto pos_quot = str.find('\"');
        while(pos_quot != std::string::npos) {
            str.replace(pos_quot, 1, "&quot;"s);
            pos_quot = str.find('\"', pos_quot + 6);
        }
        auto pos_apos = str.find('\'');
        while(pos_apos != std::string::npos) {
            str.replace(pos_apos, 1, "&apos;"s);
            pos_apos = str.find('\'', pos_apos + 6);
        }
        auto pos_lt = str.find('<');
        while(pos_lt != std::string::npos) {
            str.replace(pos_lt, 1, "&lt;"s);
            pos_lt = str.find('<', pos_lt + 4);
        }
        auto pos_gt = str.find('>');
        while(pos_gt != std::string::npos) {
            str.replace(pos_gt, 1, "&gt;"s);
            pos_gt = str.find('>', pos_gt + 4);
        }
        return str;
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;

        RenderContext context(out, 2);

        for (const auto& obj : objects_) {
            obj->Render(context.Indented());
        }

        out << "</svg>"sv;
    }

}  // namespace svg
