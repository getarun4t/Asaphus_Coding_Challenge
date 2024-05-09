/**
 * @file asaphus_coding_challenge.cpp
 * @version 1.1
 * @copyright Copyright (c) 2022 Asaphus Vision GmbH
 *
 * The goal is to implement the mechanics of a simple game and write test cases for them.
 * The rules of the game are:
 * - There are two types of boxes, green and blue.
 * - Both can absorb tokens of a given weight, which they add to their own total weight.
 * - Both are initialized with a given initial weight.
 * - After a box absorbs a token weight, it outputs a score.
 * - Green and blue boxes calculate the score in different ways:
 * - A green box calculates the score as the square of the mean of the 3 weights that it most recently absorbed (square of mean of all absorbed weights if there are fewer than 3).
 * - A blue box calculates the score as Cantor's pairing function of the smallest and largest weight that it has absorbed so far, i.e. pairing(smallest, largest), where pairing(0, 1) = 2
 * - The game is played with two green boxes with initial weights 0.0 and 0.1, and two blue boxes with initial weights 0.2 and 0.3.
 * - There is a list of input token weights. Each gets used in one turn.
 * - There are two players, A and B. Both start with a score of 0.
 * - The players take turns alternatingly. Player A starts.
 * - In each turn, the current player selects one of the boxes with the currently smallest weight, and lets it absorb the next input token weight. Each input weight gets only used once.
 * - The result of the absorption gets added to the current player's score.
 * - When all input token weights have been used up, the game ends, and the player with highest score wins.
 *
 * Task:
 * - Create a git repo for the implementation. Use the git repository to have a commit history.
 * - Implement all missing parts, marked with "TODO", including the test cases.
 * - Split the work in reasonable commits (not just one commit).
 * - Make sure the test cases succeed.
 * - Produce clean, readable code.
 *
 * Notes:
 * - Building and running the executable: g++ --std=c++14 asaphus_coding_challenge.cpp -o challenge && ./challenge
 * - Feel free to add a build system like CMake, meson, etc.
 * - Feel free to add more test cases, if you would like to test more.
 * - This file includes the header-only test framework Catch v2.13.9.
 * - A main function is not required, as it is provided by the test framework.
 */

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <vector>
#include <cmath>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

enum class BoxType { GREEN, BLUE };

class Box {
public:
    explicit Box(double initial_weight) : weight_(initial_weight) {}
    static std::unique_ptr<Box> makeGreenBox(double initial_weight);
    static std::unique_ptr<Box> makeBlueBox(double initial_weight);
    bool operator<(const Box& rhs) const { return weight_ < rhs.weight_; }

    double getScore() const { return score_; }
    double getWeight() const { return weight_; }
    void absorbWeight(double weight);

    BoxType getBoxType() const { return type_; }
    void setBoxType() {
        if (weight_ > 0.3) {
            std::cout << "Type cannot be changed once game is started" << std::endl;
        }

        if (weight_ > 0.1) {
            type_ = BoxType::BLUE;
        }
    }

protected:
    double weight_;
    double score_ = 0.0;
    std::list<double> absorbed_weights_;
    BoxType type_ = BoxType::GREEN;
};

void Box::absorbWeight(double weight) {
    if (this->getBoxType() == BoxType::GREEN) {
        absorbed_weights_.push_back(weight);
        double mean = 0;
        if (absorbed_weights_.size() >= 3) {
            auto last_three_begin = absorbed_weights_.end();
            std::advance(last_three_begin, -3);
            mean = std::accumulate(last_three_begin, absorbed_weights_.end(), 0.0) / 3;
        }
        else {
            mean = std::accumulate(absorbed_weights_.begin(), absorbed_weights_.end(), 0.0) / absorbed_weights_.size();
        }
        score_ = std::pow(mean, 2);
        this->weight_ += weight;
    }
    else {
        if (!absorbed_weights_.empty()) {
            if (absorbed_weights_.front() > weight) { absorbed_weights_.push_front(weight); }
            else if (absorbed_weights_.back() < weight) { absorbed_weights_.push_back(weight); }
            else {
                auto insertion_point = absorbed_weights_.end();
                std::advance(insertion_point, -3);
                absorbed_weights_.insert(insertion_point, weight);
            }
        }
        else {
            absorbed_weights_.push_back(weight);
        }
        score_ = ((absorbed_weights_.front() + absorbed_weights_.back()) * (absorbed_weights_.front() + absorbed_weights_.back() + 1)) / 2 + absorbed_weights_.back();
        this->weight_ += weight;
    }
}

std::unique_ptr<Box> Box::makeGreenBox(double initial_weight) {
    return std::make_unique<Box>(initial_weight);
}

std::unique_ptr<Box> Box::makeBlueBox(double initial_weight) {
    return std::make_unique<Box>(initial_weight);
}

class Player {
public:
    void takeTurn(uint32_t input_weight, std::vector<std::unique_ptr<Box>>& boxes) {
        auto min_box = std::min_element(boxes.begin(), boxes.end(), [](const auto& a, const auto& b) {
            return *a < *b;
            });
        (*min_box)->absorbWeight(static_cast<double>(input_weight));
        score_ += (*min_box)->getScore();
    }

    double getScore() const { return score_; }

private:
    double score_ = 0.0;
};


std::pair<double, double> play(const std::vector<uint32_t>& input_weights) {
    std::vector<std::unique_ptr<Box>> boxes;
    boxes.emplace_back(Box::makeGreenBox(0.0));
    boxes.emplace_back(Box::makeGreenBox(0.1));
    boxes.emplace_back(Box::makeBlueBox(0.2));
    boxes.emplace_back(Box::makeBlueBox(0.3));

    for (const auto& box : boxes) {
        box->setBoxType();
    }

    Player player_A, player_B;

    bool is_player_A_turn = true;
    for (uint32_t weight : input_weights) {
        if (is_player_A_turn) {
            player_A.takeTurn(weight, boxes);
        }
        else {
            player_B.takeTurn(weight, boxes);
        }
        is_player_A_turn = !is_player_A_turn;
    }

    std::cout << "Scores: player A " << player_A.getScore() << ", player B " << player_B.getScore() << std::endl;
    return std::make_pair(player_A.getScore(), player_B.getScore());
}


TEST_CASE("Final scores for first 4 Fibonacci numbers", "[fibonacci4]") {
    std::vector<uint32_t> inputs{ 1, 1, 2, 3 };
    auto result = play(inputs);
    REQUIRE(result.first == 13.0);
    REQUIRE(result.second == 25.0);
}

TEST_CASE("Final scores for first 8 Fibonacci numbers", "[fibonacci8]") {
    std::vector<uint32_t> inputs{ 1, 1, 2, 3, 5, 8, 13, 21 };
    auto result = play(inputs);
    REQUIRE(result.first == 155.0);
    REQUIRE(result.second == 366.25);
}

TEST_CASE("Test absorption of green box", "[green]") {
    std::vector<uint32_t> inputs{ 1, 2, 3 };
    std::unique_ptr<Box> greenBox = Box::makeGreenBox(0.0);
    greenBox->setBoxType();
    greenBox->absorbWeight(inputs[0]);
    REQUIRE(greenBox->getScore() == 1.0);
    greenBox->absorbWeight(inputs[1]);
    REQUIRE(greenBox->getScore() == 2.25);
    greenBox->absorbWeight(inputs[2]);
    REQUIRE(greenBox->getScore() == 4.0);
}

TEST_CASE("Test absorption of blue box", "[blue]") {
    std::vector<uint32_t> inputs{ 1, 2, 3 };
    std::unique_ptr<Box> blueBox = Box::makeBlueBox(0.2);
    blueBox->setBoxType();
    blueBox->absorbWeight(inputs[0]);
    REQUIRE(blueBox->getScore() == 4);
    blueBox->absorbWeight(inputs[1]);
    REQUIRE(blueBox->getScore() == 8);
    blueBox->absorbWeight(inputs[2]);
    REQUIRE(blueBox->getScore() == 13);
}

TEST_CASE("Test absorption of green box with zero weights", "[green]") {
    std::unique_ptr<Box> green_box = Box::makeGreenBox(0.0);
    REQUIRE(green_box->getScore() == 0.0);
}

TEST_CASE("Test absorption of blue box with zero weights", "[blue]") {
    std::unique_ptr<Box> blue_box = Box::makeBlueBox(0.0);
    REQUIRE(blue_box->getScore() == 0.0);
}

TEST_CASE("Test play() function with zero weights", "[play]") {
    std::vector<uint32_t> inputs;
    auto result = play(inputs);
    REQUIRE(result.first == 0.0);
    REQUIRE(result.second == 0.0);
}

TEST_CASE("Test setBoxType() function", "[play]") {
    std::vector<double> initial_weights{ 0.0, 0.1, 0.2, 0.3 };
    std::vector<BoxType> boxes{ BoxType::GREEN, BoxType::GREEN, BoxType::BLUE, BoxType::BLUE };
    for (int index = 0; index < initial_weights.size(); ++index) {
        std::unique_ptr<Box> box;
        if (initial_weights[index] < 0.2)
            box = Box::makeGreenBox(initial_weights[index]);
        else
            box = Box::makeBlueBox(initial_weights[index]);
        box->setBoxType();
        REQUIRE(box->getBoxType() == boxes[index]);
    }
}