#include <cstdlib>
#include <fstream>

#include <xargs.hpp>

#define GENERATEMAIN_VERSION "0.0.2"

extern int main(int argc, char *argv[]) {
    const auto app_name = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : nullptr;

    using namespace std;

    string filepath;
    string game_name = "sample-game";
    string script_name = "sample-game.lua";

    xargs::args args;
    args.add_arg("OUTPUT_FILEPATH", "Path to output file", [&] (const auto &v) {
        filepath = v;
    }).add_option("-n", "Application name. Default: " + game_name, [&] (const auto &v) {
        game_name = v;
    }).add_option("-s", "Launch script name. Default: " + script_name, [&] (const auto &v) {
        script_name = v;
    }).add_option("-h", "Display help", [&] () {
        puts(args.usage(argv[0]).c_str());
        exit(EXIT_SUCCESS);
    }).add_option("-v", "Version", [&] () {
        fprintf(stdout, "%s %s\n", app_name, GENERATEMAIN_VERSION);
        exit(EXIT_SUCCESS);
    });

    args.dispath(argc, argv);

    if (static_cast<size_t>(argc) < args.count()) {
        puts(args.usage(argv[0]).c_str());
        return EXIT_SUCCESS;
    }

    ofstream ofs(filepath, ofstream::out);
    if (!ofs.is_open()) {
        fprintf(stderr, "%s %s %s\n", app_name, "Can't open file", filepath.c_str());
        return EXIT_FAILURE;
    }

    const string indent = "    ";

    ofs << "// Generated by " << app_name << " " << GENERATEMAIN_VERSION << "\n"
        << "#include <ironforge.hpp>\n\n"
        << "extern int main(int argc, char* argv[]) {\n"
        << indent << "(void)argc;\n"
        << indent << "(void)argv;\n"
        << indent << "using namespace std;\n"
        << indent << "using namespace game;\n\n"
        << indent << "const auto app_name = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : nullptr;\n"
        << indent << "journal::setup_default(string{app_name} + \".log\");\n"
        << "\n"
        << indent << "return exec(get_base_path() + \"" << script_name << "\");\n"
        << "}";

    ofs.close();

    return 0;
}