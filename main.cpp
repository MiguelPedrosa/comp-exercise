#include <initializer_list>
#include <unordered_map>
#include <string>
#include <sstream>
#include <memory>
#include <utility>
#include <cstdio>



struct Block
{
    Block(std::string name) : name(std::move(name)) {}
    std::string name;
};

struct Link
{
    std::string tag;
    std::string predecessor;
    std::string successor;
};

struct Function
{

    Function(std::string name, std::string entry_block)
      : Function(std::move(name), std::move(entry_block), {}, {}) {}
    Function(std::string name, std::string entry_block, std::initializer_list<Block> blocks)
      : Function(std::move(name), std::move(entry_block), blocks, {}) {}
    Function(std::string name, std::string entry_block, std::initializer_list<Block> blocks, std::initializer_list<Link> links)
      : name(std::move(name)), entry_block_name(std::move(entry_block)) {
        for (auto block : blocks) {
            auto block_name = block.name;
            this->blocks.emplace(std::move(block_name), std::move(block));
        }
        for (auto link : links) {
            auto link_name = link.tag;
            this->links.emplace(std::move(link_name), std::move(link));
        }
    }
 
    bool add_block(Block&& block)
    {
        auto block_name = block.name;
        auto [elem, success] = this->blocks.emplace(std::move(block_name), std::move(block));
        return success;
    }

    bool remove_block(const std::string& name)
    {
        return this->blocks.erase(name);
    }

    Block& get_block(const std::string& name)
    {
        auto block= this->blocks.find(name);
        return block->second;
    }

    bool add_link(Link&& link)
    {
        auto link_name = link.tag;
        auto [elem, success] = this->links.emplace(std::move(link_name), std::move(link));
        return success;
    }

    bool remove_link(const std::string& name)
    {
        return this->links.erase(name);
    }

    Link& get_link(const std::string& name)
    {
        auto link = this->links.find(name);
        return link->second;
    }

    std::string print() const {
        std::stringstream ss;
        ss << "subgraph cluster_func_" << this->name << " {\n";
        ss << "  label = \"" << this->name << "\";\n";
        ss << "  style = rounded;\n\n";

        // 1. Define all blocks (nodes)
        for (const auto& pair : this->blocks) {
            const Block& block = pair.second;
            ss << "  \"" << block.name << "\" [shape=box];\n";
        }
        ss << "\n";

        // 2. Define all links (edges)
        for (const auto& pair : this->links) {
            const Link& link = pair.second;
            ss << "  \"" << link.predecessor << "\" -> \"" << link.successor << "\"";
            if (!link.tag.empty()) {
                ss << " [label=\"" << link.tag << "\"]";
            }
            ss << ";\n";
        }

        ss << "}\n";
        return ss.str();

    }

    const std::string name;
    std::string entry_block_name;
    std::unordered_map<std::string, Block> blocks;
    std::unordered_map<std::string, Link> links;
};

struct Module
{
    Module(std::string name) : Module(std::move(name), {}) {}
    Module(std::string name, std::initializer_list<Function> functions)
      : name(std::move(name)) {
        for (auto function : functions) {
            auto function_name = function.name;
            this->functions.emplace(std::move(function_name), std::move(function));
        }
    }

    bool add_function(Function&& function)
    {
        auto function_name = function.name;
        auto [elem, success] = this->functions.emplace(std::move(function_name), std::move(function));
        return success;
    }

    bool remove_function(const std::string& name)
    {
        return this->functions.erase(name);
    }

    Function& get_functions(const std::string& name)
    {
        auto function = this->functions.find(name);
        return function->second;
    }

    std::string print() const
    {
        std::stringstream ss;
        ss << "subgraph " << this->name
            << R"foo( {
    style=filled;
    color=lightgrey;
)foo";

        // Recursively call to_dot for all contained modules
        for (const auto& pair : this->functions) {
            ss << pair.second.print() << "\n";
        }

        ss << "}\n";
        return ss.str();
    }

    const std::string name;
    std::unordered_map<std::string, Function> functions;
};

struct Compiler
{
    Compiler() : Compiler({}) {}
    Compiler(std::initializer_list<Module> modules)
    {
        this->modules.reserve(modules.size());
        for (auto module : modules) {
            this->modules.emplace(module.name, module);
        }
    }

    bool add_module(Module&& module)
    {
        auto module_name = module.name;
        auto [elem, success] = this->modules.emplace(std::move(module_name), std::move(module));
        return success;
    }

    bool remove_module(const std::string& name)
    {
        return this->modules.erase(name);
    }

    Module& get_module(const std::string& name)
    {
        auto module = this->modules.find(name);
        return module->second;
    }

    std::string print() const
    {
        std::stringstream ss;
        ss << R"foo(
digraph CompilerIR {
    rankdir = LR;
    bgcolor = white;
)foo";

        // Recursively call to_dot for all contained modules
        for (const auto& pair : this->modules) {
            ss << pair.second.print() << "\n";
        }

        ss << "}\n";
        return ss.str();
    }

private:
    std::unordered_map<std::string, Module> modules;
};


int main()
{
    Compiler compiler {
        Module("mod1", {
            Function("func11", "", {
                    Block("func11_blk11_entry"),
                    Block("func11_blk12"),
                    Block("func11_blk13")
                }, {
                    {"true",  "func11_blk11_entry", "func11_blk12"},
                    {"",      "func11_blk12",       "func11_blk13"},
                    {"false", "func11_blk11_entry", "func11_blk13"}
            }),
            Function("func12", "func12_blk1_entry", {
                    Block("func12_blk1_entry"),
                    Block("func12_blk2"),
                    Block("func12_blk3"),
                    Block("func12_blk4")
                }, {
                    {"true",  "func12_blk1_entry", "func12_blk2"},
                    {"step",  "func12_blk2", "func12_blk3"},
                    {"loop",  "func12_blk3", "func12_blk2"},
                    {"exit",  "func12_blk3", "func12_blk4"}
            })
        }),
        Module("mod2", {
            Function("func21", "func21_blk1_entry", {
                Block("func21_blk1_entry")
            })
        }),
        Module("mod3")
    };

    std::printf("%s\n", compiler.print().c_str());
}
