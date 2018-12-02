#include <map>
#include <optional>
#include <string>
#include <vector>

class FileCacher
{
  public:
    std::optional<std::string> get_line(const std::string& file_path,
                                        const size_t line);

  private:
    void load_file(const std::string& file_path);
    std::map<std::string, std::vector<std::string>> _container;
};
