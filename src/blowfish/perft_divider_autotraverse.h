#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chessboard.h"
#include "move_generator.h"
#include "perft_divider.h"

//This Class implements a perft divider customized move generator. It stores results in a map and is 
//capable of doing system calls to stockfish //requires it to be installed locally.
//It then compares output, checks discrepancy between the tools and iterates a new perft divider search depth - 1 from issued position.

enum class SearchErrorType {
    kEnumerationDiscrepancy = 0,
    kMoveNotFound  = 1,
    kMoveNotAllowed = 2,
    kNoError = 3
};


//this turned out to be a well written class implementation, i'll try to generalize some of the functionality
class PerftDividerAutotraverse {
public : 

    typedef std::map<std::string, long long> PerftResultMap;
    typedef std::map<std::string, BoardNode*> PerftNodeMap;

    PerftDividerAutotraverse() {
        //InitFileRead();
    }

    ~PerftDividerAutotraverse() {
        command_fstream_.close();
    }

    void EnumerateDivide(const Board & board, const int & divide_depth);

    inline bool InitFileRead() {
        command_fstream_.open(command_filename_);  
        command_fstream_.clear();
        return (bool)command_fstream_;
     }

    //decodes output from ExecuteStockfishCommand into a map result. We dont have BoardNode in this entries so comparison must be
    //between PerftResultMap and PerftNodeMap
    PerftResultMap ConvertStockfishResult(std::string s) {  
        std::string delimiter = ":";

        std::vector<std::string> string_vec = SplitString(s, "\n");
        string_vec.erase(string_vec.begin(), string_vec.begin() + 1);
        string_vec.erase(string_vec.begin() + string_vec.size() - 4, string_vec.end());

        PerftResultMap result_map;
        for(auto & linestring : string_vec) {
            auto kvpair = SplitString(linestring, ":");
            result_map[kvpair[0]] = (long long)std::stoi(kvpair[1]);
        }
        return result_map;/* 
        for(auto & kvp : result_map) {
            std::cout << "key : " << kvp.first << " value : " << kvp.second << std::endl;
        } */
    }
        
    //Updates the file with perft search request information for stockfish
    void UpdateStockfishSearchQuery(const std::string fen_position, const int & depth) {      
        command_fstream_ << "position fen " + fen_position << std::endl;
        command_fstream_ << "go perft " + std::to_string(depth) << std::endl;
        command_fstream_ << "quit" << std::endl;
    }

    //pulls data from command filename into stockfish via a systemcall. output is captured in string
    std::string ExecuteStockfishCommand() {
        
        FILE* pipe = popen(("stockfish < " + command_filename_).data(), "r");
        
        if (!pipe) return "No file";
        char buffer[128];
        std::string result = "";
        while(!feof(pipe)) {
            if(fgets(buffer, 128, pipe) != nullptr)
                result += buffer;
        }
        pclose(pipe);
        return result;
    }
private: 
    //two methods to compare, one for depth > 1 where the first non matching result is returned. the other displays all discrepancies between the maps

    std::vector<std::string> SplitString(const std::string& str,
                                      const std::string& delimiter)
    {
        std::vector<std::string> strings;

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos)
        {
            strings.push_back(str.substr(prev, pos - prev));
            prev = pos + delimiter.size();
        }

        // To get the last substring (or only, if delimiter is not found)
        strings.push_back(str.substr(prev));

        return strings;
    }

    //Returns the key to progress

    //the leet code problem, for a giving N sized map M1 and a K sized map M2,
    //return all keys that are not shared.

    std::pair<SearchErrorType, std::string> CompareReturnFirstError(PerftNodeMap map1, PerftResultMap map2) {        
        for(const auto & perftnode : map2) {
            auto match = map1.find(perftnode.first); 

            if(match == map1.end()) {
                return std::pair<SearchErrorType, std::string> (SearchErrorType::kMoveNotFound, perftnode.first);
            }
            else {
                if(match->second->GetSubnodes() != perftnode.second) {
                    return std::pair<SearchErrorType, std::string> (SearchErrorType::kEnumerationDiscrepancy, perftnode.first);
                }
            }         
        }

        //case 2 we are allowing an illegal move
        for(const auto & perftnode : map1) {
            auto match = map2.find(perftnode.first); 

            if(match == map2.end()) {                
                return std::pair<SearchErrorType, std::string> (SearchErrorType::kMoveNotAllowed, perftnode.first);
            }                   
        }

        return std::pair<SearchErrorType, std::string> (SearchErrorType::kNoError, "this value");
    }

    //Displays all issues found, this occurs in depth 1 (bottom)
    void CompareAny(PerftNodeMap map1, PerftResultMap map2) {
/*         print("Our generator");
        for(const auto & m1nodes : map1) {
            std::cout << m1nodes.first << " : " << m1nodes.second->GetSubnodes()  << std::endl;
        }

        print("Stockfish generator");
        for(const auto & m1nodes : map2) {
            std::cout << m1nodes.first << " : " << m1nodes.second  << std::endl;
        } */


        


        print(map1.size());
        print(map2.size());

        //case 1 we are not supporting all legal moves
        bool found_issue = false;
        for(const auto & perftnode : map2) {
            auto match = map1.find(perftnode.first); 

            if(match == map1.end()) {
                found_issue = true;
                std::cout << "Move missing : " << perftnode.first << std::endl;
            }                   
        }
        if(found_issue) return;

        //case 2 we are allowing an illegal move
        for(const auto & perftnode : map1) {
            auto match = map2.find(perftnode.first); 

            if(match == map2.end()) {                
                std::cout << "Allowing illegal move : " << perftnode.first << std::endl;
            }                   
        } 
    }

private:
    std::string      command_filename_ = "perft_autotraverse_commandcalls.txt";
    std::ofstream    command_fstream_;
};