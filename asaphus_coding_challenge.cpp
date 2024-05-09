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
	void setBoxType(BoxType newType) { type_ = newType; }

protected:
	double weight_;
	double score_ = 0.0;
	std::list<double> absorbed_weights_;
	BoxType type_ = BoxType::GREEN;
};

//Method to absorb weight into the box
void Box::absorbWeight(double weight) {
	if (this->getBoxType() == BoxType::GREEN) {
		absorbed_weights_.emplace_back(weight);
		double mean = 0;
		auto list_size = absorbed_weights_.size();
		auto last_item_iter = absorbed_weights_.end();
		if (list_size >= 3) {
			auto third_last_item_iter = last_item_iter;
			std::advance(third_last_item_iter, -3);
			mean = std::accumulate(third_last_item_iter, last_item_iter, 0.0) / 3; //Calculating mean of last three elements when box absorbed more than 3 weights
		}
		else {
			mean = std::accumulate(absorbed_weights_.begin(), last_item_iter, 0.0) / list_size; //Calculating mean of all elements when box absorbed less than 3 weights
		}
		score_ = std::pow(mean, 2);
		this->weight_ += weight;
	}
	else {
		if (!absorbed_weights_.empty()) {
			if (absorbed_weights_.front() > weight) { absorbed_weights_.emplace_front(weight); } // adding smallest weight as first element of list
			else if (absorbed_weights_.back() < weight) { absorbed_weights_.emplace_back(weight); } // adding largest weight as last element of list
			else {
				auto insertion_point = absorbed_weights_.end();
				std::advance(insertion_point, -3);
				absorbed_weights_.insert(insertion_point, weight); //adding all other weights in the middle of the list
			}
		}
		else {
			absorbed_weights_.emplace_back(weight);  //adding first element to the list
		}
		auto first_item = absorbed_weights_.front();
		auto last_item = absorbed_weights_.back();
		double sum = first_item + last_item;
		score_ = ((sum) * (sum + 1)) / 2 + last_item; //calculating Cantor's pairing function of the smallest and largest weight 
		this->weight_ += weight;
	}
}

//Initializing a green box
std::unique_ptr<Box> Box::makeGreenBox(double initial_weight) {
	return std::make_unique<Box>(initial_weight);
}

//Initializing a blue box
std::unique_ptr<Box> Box::makeBlueBox(double initial_weight) {
	std::unique_ptr<Box> blueBox = std::make_unique<Box>(initial_weight);
	blueBox->setBoxType(BoxType::BLUE);
	return blueBox;
}

class Player {
public:
	void takeTurn(uint32_t input_weight, std::vector<std::unique_ptr<Box>>& boxes) {
		//finding the box with the lowest weight
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

	Player player_A, player_B;

	//Logic for Players taking turns
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
	std::vector<uint32_t> inputs{ 1, 2, 3, 4 };
	std::unique_ptr<Box> greenBox = Box::makeGreenBox(0.0);
	double expected_scores[] = { 1.0, 2.25, 4.0, 9.0 };

	for (size_t i = 0; i < inputs.size(); ++i) {
		greenBox->absorbWeight(inputs[i]);
		REQUIRE(greenBox->getScore() == expected_scores[i]);
	}
}

TEST_CASE("Test absorption of blue box", "[blue]") {
	std::vector<uint32_t> inputs{ 2, 1, 4, 3 };
	std::unique_ptr<Box> blueBox = Box::makeBlueBox(0.2);
	double expected_scores[] = { 12, 8, 19, 32 };

	for (size_t i = 0; i < inputs.size(); ++i) {
		blueBox->absorbWeight(inputs[i]);
		REQUIRE(blueBox->getScore() == expected_scores[i]);
	}
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