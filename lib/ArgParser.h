#pragma once

#include <string>
#include <vector>

namespace ArgumentParser {

    struct StringArgument {
        std::string argument_name;
        char argument_short_name;
        bool is_empty;

        std::string argument_value;
        std::string* store_value;

        bool is_multi_value;
        int amount_of_arguments;
        int amount_of_arguments_in;
        bool is_positional;
        std::vector<std::string> argument_values;
        std::vector<std::string>* store_values;

        std::string logic;

        StringArgument();
    };

    struct IntArgument {
        std::string argument_name;
        char argument_short_name;
        bool is_empty;

        int argument_value;
        int* store_value;

        bool is_multi_value;
        int amount_of_arguments;
        int amount_of_arguments_in;
        bool is_positional;
        std::vector<int> argument_values;
        std::vector<int>* store_values;

        std::string logic;

        IntArgument();
    };

    struct Flag {
        std::string flag_name;
        char flag_short_name;
        bool flag_value;
        bool* store_value;
        std::string logic;

        Flag();
    };

    struct HelpParam {
        std::string help_name;
        char help_short_name;
        std::string logic;
        bool is_help_init;

        HelpParam();
    };

    class ArgParser {
    private:
        std::string parser_name;
        std::string info;

        std::vector<StringArgument> string_arguments; // last added param = 1
        std::vector<IntArgument> int_arguments; // last added param = 2
        std::vector<Flag> flags; // last added param = 3

        HelpParam help;

        int last_added_param;
    public:
        ArgParser(std::string name);

        ArgParser(std::string name, std::string info);

        //--------------------------- Parsing functions ---------------------------

        bool Parse(int argc, char** argv);

        bool Parse(std::vector<std::string> input_line);

        //--------------------------- Functions for string ---------------------------
        ArgParser& AddStringArgument(std::string name);

        ArgParser& AddStringArgument(char short_name, std::string name);

        ArgParser& AddStringArgument(char short_name, std::string name, std::string about);

        ArgParser& AddStringArgument(std::string name, std::string about);

        std::string GetStringValue(std::string name);

        std::string GetStringValue(std::string name, int number);

        void StoreValue(std::string& variable);

        void StoreValues(std::vector<std::string>& variable);

        //--------------------------- Functions for int ---------------------------
        ArgParser& AddIntArgument(std::string name);

        ArgParser& AddIntArgument(char short_name, std::string name);

        ArgParser& AddIntArgument(char short_name, std::string name, std::string about);

        ArgParser& AddIntArgument(std::string name, std::string about);

        int GetIntValue(std::string name);

        int GetIntValue(std::string name, int number);

        void StoreValue(int& variable);

        void StoreValues(std::vector<int>& variable);

        //--------------------------- Functions for flag ---------------------------
        ArgParser& AddFlag(std::string name);

        ArgParser& AddFlag(char short_name, std::string name);

        ArgParser& AddFlag(char short_name, std::string name, std::string about);

        ArgParser& AddFlag(std::string name, std::string about);

        bool GetFlag(std::string name);

        void Default(bool value);

        void StoreValue(bool& variable);

        //--------------------------- Help functions ---------------------------
        void AddHelp(char short_name, std::string name, std::string about);

        std::string HelpDescription();

        bool Help();

        //--------------------------- Functions for different params ---------------------------
        ArgParser& MultiValue(int count);

        ArgParser& MultiValue();

        ArgParser& Positional();

        void Default(char* value);
    };
}