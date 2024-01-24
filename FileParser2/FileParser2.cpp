#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cctype>

bool validateFile(const std::string& filename) {
    std::ifstream file(filename);

    // Check if file is able to open
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open or read the file." << std::endl;
        return false;
    }

    // Check if the file has a .txt extension
    if (filename.rfind(".txt") == std::string::npos) {
        std::cerr << "Error: File " << filename << " is not a .txt file." << std::endl;
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Check for file content
    if (fileSize == 0) {
        std::cerr << "Error: File is empty." << std::endl;
        return false;
    }

    // Limit the number of bytes to 4096
    if (fileSize > 4096) {
        std::cerr << "Error: File size is greater than 4096 bytes." << std::endl;
        return false;
    }

    file.close();

    return true;
}

// Function to read the file content
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: File " << filename << " not found or cannot be opened." << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to find the next non-space character
size_t findNextNonSpace(const std::string& content, size_t start) {
    for (size_t i = start; i < content.size(); ++i) {
        if (!std::isspace(content[i])) {
            return i;
        }
    }
    return std::string::npos;
}

/// Function to tokenize the content
std::map<std::string, int> tokenizeContent(const std::string& content) {
    std::map<std::string, int> tokenCount;

    std::string word;
    bool isInsideWord = false;

    for (size_t i = 0; i < content.size(); ++i) {
        char ch = content[i];

        if (std::isalnum(ch)) {
            // Include alphanumeric characters in the word
            word += ch;
            isInsideWord = true;
        }
        else if (ch == '-' && isInsideWord) {
            // Include hyphens only if they are within a word and not at the end
            size_t nextNonSpace = findNextNonSpace(content, i + 1);
            if (!word.empty() && nextNonSpace != std::string::npos && nextNonSpace > i + 1) {
                word += ch;
            }
        }
        else if (ch == '\'' && isInsideWord) {
            // Include apostrophes within a word but not at the end
            size_t nextNonSpace = findNextNonSpace(content, i + 1);
            if (!word.empty() && nextNonSpace != std::string::npos && nextNonSpace > i + 1 && nextNonSpace < content.size() && isalnum(content[nextNonSpace])) {
                // Exclude the apostrophe from the word
                tokenCount[word]++;
                word.clear(); // Clear the word for the next one
                isInsideWord = false;
            }
        }
        else if (isInsideWord) {
            // Non-alphanumeric character encountered, consider it as the end of a word
            tokenCount[word]++;
            word.clear(); // Clear the word for the next one
            isInsideWord = false;
        }
        // If the character is not alphanumeric, a hyphen, or an apostrophe and the word is empty, skip it
    }

    // Check for the last word in the content
    if (!word.empty()) {
        tokenCount[word]++;
    }

    return tokenCount;
}

// Function to sort tokens by occurrence
std::vector<std::pair<std::string, int>> sortTokensByOccurrence(const std::map<std::string, int>& tokenCount) {
    std::vector<std::pair<std::string, int>> sortedTokens(tokenCount.begin(), tokenCount.end());
    std::sort(sortedTokens.begin(), sortedTokens.end(),
        [&tokenCount](const auto& a, const auto& b) {
            return a.second > b.second;
        });

    return sortedTokens;
}

// Function to find the token with the most repeated characters
std::pair<std::string, int> findTokenWithMostRepeatedCharacters(const std::vector<std::pair<std::string, int>>& sortedTokens) {
    int maxRepeatedCount = 0;
    std::string maxRepeatedToken;

    for (const auto& mappedTokens : sortedTokens) {
        const std::string& token = mappedTokens.first;

        std::map<char, int> charCount;
        for (char ch : token) {
            charCount[ch]++;
        }

        auto maxCharCount = std::max_element(charCount.begin(), charCount.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });

        if (maxCharCount->second > maxRepeatedCount) {
            maxRepeatedCount = maxCharCount->second;
            maxRepeatedToken = token;
        }
    }

    return std::make_pair(maxRepeatedToken, maxRepeatedCount);
}

// Function to print token occurrences
void printTokenOccurrences(const std::map<std::string, int>& tokenCount) {
    std::cout << "\nToken Occurences" << std::endl;
    std::cout << std::left << std::setw(20) << "Token" << std::setw(10) << "Occurrences" << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    for (const auto& mappedTokens : tokenCount) {
        std::cout << std::setw(20) << mappedTokens.first << std::setw(10) << mappedTokens.second << std::endl;
    }
}

// Function to print sorted tokens
void printSortedTokens(const std::vector<std::pair<std::string, int>>& sortedTokens) {
    std::cout << "\nSorted tokens by occurrence:" << std::endl;
    for (const auto& mappedTokens : sortedTokens) {
        std::cout << mappedTokens.first << ": " << mappedTokens.second << std::endl;
    }
}

// Function to print top tokens with the most characters
void printTopTokens(const std::vector<std::pair<std::string, int>>& sortedTokens, int numTokens) {
    std::vector<std::pair<std::string, int>> topTokens(sortedTokens.begin(), sortedTokens.begin() + numTokens);

    std::partial_sort(topTokens.begin(), topTokens.end(), topTokens.end(),
        [](const auto& a, const auto& b) {
            return a.first.length() > b.first.length();
        });

    std::cout << "\nTop " <<numTokens<< " tokens with the most characters : " << std::endl;
    for (const auto& token : topTokens) {
        std::cout << token.first << ": " << token.first.length() << " characters" << std::endl;
    }
}

// Function to print top tokens based on the number of tokens in the file
void printTopTokensBasedOnFile(const std::vector<std::pair<std::string, int>>& sortedTokens) {
    int numTokens = std::min(static_cast<int>(sortedTokens.size()), 10); // Display up to 10 tokens or all tokens if less than 10
    printTopTokens(sortedTokens, numTokens);
}

// Function to process the file
void processFile(const std::string& filename) {
    if (!validateFile(filename)) {
        return;
    }

    std::string content = readFile(filename);
    if (content.empty()) {
        return;
    }

    std::map<std::string, int> tokenCount = tokenizeContent(content);

    printTokenOccurrences(tokenCount);

    std::vector<std::pair<std::string, int>> sortedTokens = sortTokensByOccurrence(tokenCount);

    printSortedTokens(sortedTokens);

    printTopTokensBasedOnFile(sortedTokens);

    auto result = findTokenWithMostRepeatedCharacters(sortedTokens);
    std::cout << "\nToken with the most repeated characters: " << result.first
        << ", Repeated Characters Count: " << result.second << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    processFile(filename);

    return 0;
}
