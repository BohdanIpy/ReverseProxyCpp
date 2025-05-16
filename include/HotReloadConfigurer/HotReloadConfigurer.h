//
// Created by geth on 5/3/25.
//

#ifndef HOTRELOADCONFIGURER_H
#define HOTRELOADCONFIGURER_H

#include <map>
#include <thread>
#include <memory>
#include <filesystem>
#include <functional>


#include <openssl/sha.h>
#include <openssl/evp.h>

namespace fs = std::filesystem;

namespace ht {
    class HotReloadConfigurer {
        class ConfigurerBuilder {
            fs::path m_path;
        public:
            ConfigurerBuilder() = default;

            bool setPath(const fs::path& path);

            std::unique_ptr<HotReloadConfigurer> build();
        };

    public:

        ConfigurerBuilder builder();

    private:

        friend class ConfigurerBuilder;

        explicit HotReloadConfigurer(const fs::path& pathToConfig);

        void loadConfiguration();

        // sender to the inside service
        std::map<int, std::string> m_services;

        fs::path m_pathConfigFile;

        std::unique_ptr<std::thread> m_reloadConfigurationThread;

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> m_mdCTX;

        const EVP_MD *m_digest = nullptr;

        std::string m_hashStr;

        // listener from outsides
        std::map<int, std::string> m_listeningStreams;
    };
};

#endif //HOTRELOADCONFIGURER_H
