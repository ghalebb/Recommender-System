// RecommenderSystem.h
//
// Created by galeb on 6/21/2020.
//

#ifndef EX5_RECOMMENDERSYSTEM_H
#define EX5_RECOMMENDERSYSTEM_H

// ------------------------------ includes ------------------------------
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>

// -------------------------- const definitions -------------------------
/**
 * @def  FILE_ERR_MSG "Unable to open file "
 * @brief A macro that used when there are files opening error.
 */
#define FILE_ERR_MSG "Unable to open file "

/**
 * @def  UND_MSG "USER NOT FOUND"
 * @brief A macro which represent an error message when search non existed username.
 */
#define UND_MSG "USER NOT FOUND"

/**
 * @def  NOT_RANKED_FILM "NA"
 * @brief A macro which represents not ranked film to specific user.
 */
#define NOT_RANKED_FILM "NA"

/**
 * @def  METHOD_FAILURE -1
 * @brief A macro which represents method failure value
 */
#define METHOD_FAILURE -1

/**
 * @def  METHOD_SUCCESS 0
 * @brief A macro which represents method success value
 */
#define METHOD_SUCCESS 0

/**
 * @def  NA_VALUE 0.0
 * @brief A macro which represents not ranked films value (internal use).
 */
#define NA_VALUE 0.0


using std::string;
using std::cerr;
using std::ifstream;
using std::vector;
using std::pair;
using std::unordered_map;


/** @class RecommenderSystem a class represents a cinema system, which recommends films
 * to its costumers according to their ranking to other movies*/
class RecommenderSystem
{
public:
	/** Reads the data of films features, rankings and users' rankings into data structures
	 * @param moviesAttributesFilePath a path to the movies features and ranks file.
	 * @param userRanksFilePath path to user's rankings file.
	 * @return 0 on success, -1 otherwise.
	 * */
	int loadData(const string &moviesAttributesFilePath, const string &userRanksFilePath);

	/** This function recommends the user a film from the list according to his rankings
	 * @param userName the costumer name to recommend for
	 * @return film name on success, USER NOT FOUND on failure.
	 * */
	string recommendByContent(const string &userName);

	/** This function predict a film to the user according to his rankings and the films feature
	 * @param movieName not ranked film by the user
	 * @param userName the costumer name
	 * @param k the k'th number of close ranked-by-the-user films related to movieName
	 * @return the predicted rank in double.
	 * */
	double predictMovieScoreForUser(const string &movieName, const string &userName, int k);

	/** Recommending a not ranked film to the user.
	 * @param userName the costumer name.
	 * @return k the k'th number of close ranked-by-the-user films related to movieName.
	 * */
	string recommendByCF(const string &userName, int k);

	/** Friend function, Sorting pairs of string and double
	 * @param var1 a pair of string and double
	 * @param var2 a pair of string and double
	 * @return true if var1 is greater than var2, false otherwise.
	 * */
	friend bool pairDoubleSort(const std::pair<string, double> &var1,
							   const std::pair<string, double> &var2);

	/** friend function, calculating the norm of a vector.
	 * @param vec a vector of doubles.
	 * @return a double which represents the norm.
	 * */
	friend double norm(const vector<double> &vec);

private:

	unordered_map<string, vector<double>> _moviesScores; /**< the movies features and rankings. */

	unordered_map<string, double> _normedMoviesVectors; /**< a map of the _moviesScores vectors' norm.*/

	vector<string> _rankedFilmOrder; /**< the order of the ranked films by costumers. */

	unordered_map<string, unordered_map<string, double>> _usersRating; /**< costumers' ranking to films
 													* (may be not ranked). */

	/** adding vector and multiplication with scalar to other vector.
	 * @param scalar double scalar.
	 * @param source the added vector.
	 * @param vector to vector to add to.
	 * */
	static void _addToPreferenceVector(const vector<double> &source, vector<double> &vector,
				                       double scalar);

	/** Scalar product for two double vectors.
	 * @param vecA, vecB two double vectors with the same size.
	 * @return double which represents the dot product.
	 * */
	static double _dotProduct(const vector<double> &vecA, const vector<double> &vecB);

	/** Reading the movies features file and store the films and their values into a map.
	 * @param moviesAttributesFilePath the film attributes file path.
	 * @return 0 on success, -1 otherwise.
	 * */
	int _readMoviesAttributesFile(const string &moviesAttributesFilePath);

	/**Reading the the user's ranking file and store the users, films and their values into a map.
	 * @param userRanksFilePath the rankings file path.
	 * @return 0 on success, -1 otherwise.
	 * */
	int _readUserRanksFile(const string &userRanksFilePath);


};


#endif //EX5_RECOMMENDERSYSTEM_H
