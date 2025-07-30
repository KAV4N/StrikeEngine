#include "Asset.h"

namespace StrikeEngine {

    Asset::Asset(const std::string& id)
        : m_id(id) {
    }

    const std::string& Asset::GetId() const {
        return m_id;
    }



    void Asset::SetPath(const std::filesystem::path& path) {
        m_path = path;
    }

    const std::filesystem::path& Asset::GetPath() const {
        return m_path;
    }

}
