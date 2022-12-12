#include "ArgParser.h"

//--------------------------- Constructors ---------------------------

ArgumentParser::StringArgument::StringArgument() {
    argument_name = "";
    argument_short_name = '\0';
    is_empty = true;

    argument_value = "";
    store_value = nullptr;

    is_multi_value = false;
    amount_of_arguments = 0;
    amount_of_arguments_in = 0;
    is_positional = false;
    argument_values.resize(0);
    store_values = nullptr;

    logic = "";
}

ArgumentParser::IntArgument::IntArgument() {
    argument_name = "";
    argument_short_name = '\0';
    is_empty = true;

    argument_value = 0;
    store_value = nullptr;

    is_multi_value = false;
    amount_of_arguments_in = 0;
    amount_of_arguments = 0;
    is_positional = false;
    argument_values.resize(0);
    store_values = nullptr;

    logic = "";
}

ArgumentParser::Flag::Flag() {
    flag_name = "";
    flag_short_name = '\0';
    flag_value = false;
    store_value = nullptr;
    logic = "";
}

ArgumentParser::HelpParam::HelpParam() {
    help_name = "";
    help_short_name = '\0';
    logic = "";
    is_help_init = false;
}

ArgumentParser::ArgParser::ArgParser(std::string name) {
    parser_name = name;
    info = "";
    string_arguments.resize(0);
    int_arguments.resize(0);
    flags.resize(0);
    last_added_param = -1;
}

ArgumentParser::ArgParser::ArgParser(std::string name, std::string info) {
    parser_name = name;
    info = info;
    string_arguments.resize(0);
    int_arguments.resize(0);
    flags.resize(0);
    last_added_param = -1;
}

//--------------------------- Parsing functions ---------------------------

bool ArgumentParser::ArgParser::Parse(std::vector<std::string> input_line) {
    bool is_parse = true;
    bool is_help = false;
    bool is_positional = false;


    //check is it any Positional arguments
    for (int i = 0; i < int_arguments.size(); i++) {
        if (int_arguments[i].is_positional) {
            for (int j = 1; j < input_line.size(); j++) {
                if (int_arguments[i].store_values == nullptr) {
                    int_arguments[i].argument_values.push_back(std::stoi(input_line[j]));
                    int_arguments[i].amount_of_arguments_in++;
                } else {
                    int_arguments[i].store_values->push_back(std::stoi(input_line[j]));
                    int_arguments[i].amount_of_arguments_in++;
                }
            }
            break;
        }
    }

    for (int i = 0; i < string_arguments.size(); i++) {
        if (string_arguments[i].is_positional) {
            for (int j = 1; j < input_line.size(); j++) {
                if (string_arguments[i].store_values == nullptr) {
                    string_arguments[i].argument_values.push_back(input_line[j]);
                    string_arguments[i].amount_of_arguments_in++;
                } else {
                    string_arguments[i].store_values->push_back(input_line[j]);
                    string_arguments[i].amount_of_arguments_in++;
                }
            }
            break;
        }
    }

    // if no positional argument
    if (!is_positional) {
        for (int i = 0; i < input_line.size(); i++) {
            // if element starts with "--"
            if (input_line[i].substr(0, 2) == "--") {
                input_line[i].erase(0, 2);
                std::string buffer = input_line[i].substr(0, input_line[i].find('='));
                input_line[i].erase(0, input_line[i].find('=') + 1);
                //check for help function
                if (buffer == help.help_name) {
                    is_help = true;
                    break;
                }
                // -- + string
                for (int j = 0; j < string_arguments.size(); j++) {
                    if (buffer == string_arguments[j].argument_name) {
                        if (!string_arguments[j].is_multi_value) {
                            if (string_arguments[j].store_value != nullptr) {
                                *string_arguments[j].store_value = input_line[i];
                            } else {
                                string_arguments[j].argument_value = input_line[i];
                            }
                            string_arguments[j].is_empty = false;
                        } else {
                            if (string_arguments[j].store_values == nullptr) {
                                string_arguments[j].argument_values.push_back(input_line[i]);
                            } else {
                                string_arguments[j].store_values->push_back(input_line[i]);
                            }
                        }
                    }
                }
                // -- + int
                for (int j = 0; j < int_arguments.size(); j++) {
                    if (buffer == int_arguments[j].argument_name) {
                        if (!int_arguments[j].is_multi_value) {
                            if (int_arguments[j].store_value != nullptr) {
                                *int_arguments[j].store_value = std::stoi(input_line[i]);
                            } else {
                                int_arguments[j].argument_value = std::stoi(input_line[i]);
                            }
                            int_arguments[j].is_empty = false;
                        } else {
                            if (int_arguments[j].store_values == nullptr) {
                                int_arguments[j].argument_values.push_back(std::stoi(input_line[i]));
                                int_arguments[j].amount_of_arguments_in++;
                            } else {
                                int_arguments[j].store_values->push_back(std::stoi(input_line[i]));
                                int_arguments[j].amount_of_arguments_in++;
                            }
                        }
                    }
                }
                // -- + flag
                for (int j = 0; j < flags.size(); j++) {
                    if (buffer == flags[j].flag_name) {
                        if (flags[j].store_value != nullptr) {
                            *flags[j].store_value = true;
                        } else {
                            flags[j].flag_value = true;
                        }
                    }
                }

            } else if (input_line[i][0] == '-') { // if element starts with "-"
                // check for help function
                if (input_line[i][1] == help.help_short_name) {
                    is_help = true;
                    break;
                }
                // check all short flags
                for (int k = 1; k <= input_line.size(); k++) {
                    for (int j = 0; j < flags.size(); j++) {
                        if (input_line[i][k] == flags[j].flag_short_name) {
                            if (flags[j].store_value != nullptr) {
                                *flags[j].store_value = true;
                            } else {
                                flags[j].flag_value = true;
                            }
                        }
                    }
                }

                char buffer = input_line[i][1];
                input_line[i].erase(0, 3);
                // - + string
                for (int j = 0; j < string_arguments.size(); j++) {
                    if (buffer == string_arguments[j].argument_short_name) {
                        if (string_arguments[j].store_value != nullptr) {
                            *string_arguments[j].store_value = input_line[i];
                        } else {
                            string_arguments[j].argument_value = input_line[i];
                        }
                        string_arguments[j].is_empty = false;
                    }
                }
                // - + int
                for (int j = 0; j < int_arguments.size(); j++) {
                    if (buffer == int_arguments[j].argument_short_name) {
                        if (int_arguments[j].store_value != nullptr) {
                            *int_arguments[j].store_value = std::stoi(input_line[i]);
                        } else {
                            int_arguments[j].argument_value = std::stoi(input_line[i]);
                        }
                        int_arguments[j].is_empty = false;
                    }
                }
            }
        }
    }

    // Call Help menu
    if (is_help) {
        HelpDescription();
        return true;
    }

    // if wrong parser of string
    for (int i = 0; i < string_arguments.size(); i++) {
        if (!string_arguments[i].is_multi_value && string_arguments[i].is_empty) {
            is_parse = false;
        }
        if (string_arguments[i].is_multi_value &&
            string_arguments[i].amount_of_arguments_in < string_arguments[i].amount_of_arguments) {
            is_parse = false;
        }
    }

    // if wrong parser of int
    for (int i = 0; i < int_arguments.size(); i++) {
        if (!int_arguments[i].is_multi_value && int_arguments[i].is_empty) {
            is_parse = false;
        }
        if (int_arguments[i].is_multi_value &&
            int_arguments[i].amount_of_arguments_in < int_arguments[i].amount_of_arguments) {
            is_parse = false;
        }
    }

    return is_parse;
}

bool ArgumentParser::ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }

    return Parse(args);
}

//--------------------------- Functions for string ---------------------------

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddStringArgument(std::string name) {
    StringArgument input;
    input.argument_name = name;
    string_arguments.push_back(input);

    last_added_param = 1;

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddStringArgument(char short_name, std::string name) {
    StringArgument input;
    input.argument_short_name = short_name;
    input.argument_name = name;
    string_arguments.push_back(input);

    last_added_param = 1;

    return *this;
}

ArgumentParser::ArgParser&
ArgumentParser::ArgParser::AddStringArgument(char short_name, std::string name, std::string about) {
    StringArgument input;
    input.argument_short_name = short_name;
    input.argument_name = name;
    input.logic = about;
    string_arguments.push_back(input);

    last_added_param = 1;

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddStringArgument(std::string name, std::string about) {
    StringArgument input;
    input.argument_name = name;
    input.logic = about;
    string_arguments.push_back(input);

    last_added_param = 1;

    return *this;
}

std::string ArgumentParser::ArgParser::GetStringValue(std::string name) {
    for (int i = 0; i < string_arguments.size(); i++) {
        if (string_arguments[i].argument_name == name) {
            return string_arguments[i].argument_value;
        }
    }
}

std::string ArgumentParser::ArgParser::GetStringValue(std::string name, int number) {
    for (int i = 0; i < string_arguments.size(); i++) {
        if (string_arguments[i].argument_name == name) {
            if (string_arguments[i].store_values == nullptr) {
                return string_arguments[i].argument_values[number];
            } else {
                return string_arguments[i].store_values->operator[](number);
            }
        }
    }
}

void ArgumentParser::ArgParser::StoreValue(std::string& variable) {
    string_arguments[string_arguments.size() - 1].store_value = &variable;
}

void ArgumentParser::ArgParser::StoreValues(std::vector<std::string>& variable) {
    string_arguments[string_arguments.size() - 1].store_values = &variable;
}

//--------------------------- Functions for int ---------------------------

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddIntArgument(std::string name) {
    IntArgument input;
    input.argument_name = name;
    int_arguments.push_back(input);

    last_added_param = 2;

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddIntArgument(char short_name, std::string name) {
    IntArgument input;
    input.argument_short_name = short_name;
    input.argument_name = name;
    int_arguments.push_back(input);

    last_added_param = 2;

    return *this;
}

ArgumentParser::ArgParser&
ArgumentParser::ArgParser::AddIntArgument(char short_name, std::string name, std::string about) {
    IntArgument input;
    input.argument_short_name = short_name;
    input.argument_name = name;
    input.logic = about;
    int_arguments.push_back(input);

    last_added_param = 2;

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddIntArgument(std::string name, std::string about) {
    IntArgument input;
    input.argument_name = name;
    input.logic = about;
    int_arguments.push_back(input);

    last_added_param = 2;

    return *this;
}

int ArgumentParser::ArgParser::GetIntValue(std::string name) {
    for (int i = 0; i < int_arguments.size(); i++) {
        if (int_arguments[i].argument_name == name) {
            return int_arguments[i].argument_value;
        }
    }
}

int ArgumentParser::ArgParser::GetIntValue(std::string name, int number) {
    for (int i = 0; i < int_arguments.size(); i++) {
        if (int_arguments[i].argument_name == name) {
            if (int_arguments[i].store_values == nullptr) {
                return int_arguments[i].argument_values[number];
            } else {
                return int_arguments[i].store_values->operator[](number);
            }
        }
    }
}

void ArgumentParser::ArgParser::StoreValue(int& variable) {
    int_arguments[int_arguments.size() - 1].store_value = &variable;
}

void ArgumentParser::ArgParser::StoreValues(std::vector<int>& variable) {
    int_arguments[int_arguments.size() - 1].store_values = &variable;
}

//--------------------------- Functions for flag ---------------------------

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddFlag(std::string name) {
    Flag input;
    input.flag_name = name;
    flags.push_back(input);

    last_added_param = 3;

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddFlag(char short_name, std::string name) {
    Flag input;
    input.flag_short_name = short_name;
    input.flag_name = name;
    flags.push_back(input);

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddFlag(char short_name, std::string name, std::string about) {
    Flag input;
    input.flag_short_name = short_name;
    input.flag_name = name;
    input.logic = about;
    flags.push_back(input);

    last_added_param = 3;

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddFlag(std::string name, std::string about) {
    Flag input;
    input.flag_name = name;
    input.logic = about;
    flags.push_back(input);

    last_added_param = 3;

    return *this;
}

bool ArgumentParser::ArgParser::GetFlag(std::string name) {
    for (int i = 0; i < flags.size(); i++) {
        if (flags[i].flag_name == name) {
            return flags[i].flag_value;
        }
    }
}

void ArgumentParser::ArgParser::Default(bool value) {
    flags[flags.size() - 1].flag_value = value;
}

void ArgumentParser::ArgParser::StoreValue(bool& variable) {
    flags[flags.size() - 1].store_value = &variable;
}

//--------------------------- Help functions ---------------------------

void ArgumentParser::ArgParser::AddHelp(char short_name, std::string name, std::string about) {
    help.is_help_init = true;
    help.logic = about;
    help.help_name = name;
    help.help_short_name = short_name;
}

std::string ArgumentParser::ArgParser::HelpDescription() {
    std::string output = "Program " + parser_name + "\n" + info + "\n";
    output = output + "String arguments:\n";
    for (int i = 0; i < string_arguments.size(); i++) {
        if (string_arguments[i].argument_short_name != '\0') {
            output = output + string_arguments[i].argument_short_name + " ";
        }
        output = output + string_arguments[i].argument_name + " " + string_arguments[i].logic + "\n";
    }
    output = output + "Int arguments:\n";
    for (int i = 0; i < int_arguments.size(); i++) {
        if (int_arguments[i].argument_short_name != '\0') {
            output = output + int_arguments[i].argument_short_name + " ";
        }
        output = output + int_arguments[i].argument_name + " " + int_arguments[i].logic + "\n";
    }
    output = output + "Flags:\n";
    for (int i = 0; i < flags.size(); i++) {
        if (flags[i].flag_short_name != '\0') {
            output = output + flags[i].flag_short_name + " ";
        }
        output = output + flags[i].flag_name + " " + flags[i].logic + "\n";
    }

    return output;
}

bool ArgumentParser::ArgParser::Help() {
    return help.is_help_init;
}

//--------------------------- Functions for different params ---------------------------

// This is functions for IntArgument and StringArgument
ArgumentParser::ArgParser& ArgumentParser::ArgParser::MultiValue(int count) {
    if (last_added_param == 1) {
        string_arguments[string_arguments.size() - 1].is_multi_value = true;
        string_arguments[string_arguments.size() - 1].amount_of_arguments = count;
    }
    if (last_added_param == 2) {
        int_arguments[int_arguments.size() - 1].is_multi_value = true;
        int_arguments[int_arguments.size() - 1].amount_of_arguments = count;
    }

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::MultiValue() {
    switch (last_added_param) {
        case 1:
            string_arguments[string_arguments.size() - 1].is_multi_value = true;
            string_arguments[string_arguments.size() - 1].amount_of_arguments = 1;
        case 2:
            int_arguments[int_arguments.size() - 1].is_multi_value = true;
            int_arguments[int_arguments.size() - 1].amount_of_arguments = 1;
    }

    return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::Positional() {
    switch (last_added_param) {
        case 1:
            string_arguments[string_arguments.size() - 1].is_positional = true;
        case 2:
            int_arguments[int_arguments.size() - 1].is_positional = true;
    }

    return *this;
}

void ArgumentParser::ArgParser::Default(char* value) {
    if (last_added_param == 1) {
        string_arguments[string_arguments.size() - 1].argument_value = value;
        string_arguments[string_arguments.size() - 1].is_empty = false;
    }

    if (last_added_param == 2) {
        int_arguments[int_arguments.size() - 1].argument_value = std::stoi(value);
        int_arguments[int_arguments.size() - 1].is_empty = false;
    }
}
