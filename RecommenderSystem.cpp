// RecommenderSystem.cpp
//
// Created by galeb on 6/25/2020.
//

// ------------------------------ includes ------------------------------
#include <climits>
#include <cmath>
#include <numeric>
#include "RecommenderSystem.h"

/** adding vector and multiplication with scalar to other vector.
	 * @param scalar double scalar.
	 * @param source the added vector.
	 * @param vector to vector to add to.
	 * */
void RecommenderSystem::_addToPreferenceVector(const vector<double> &source,
											   vector<double> &vector,
											   double scalar)
{
	const int sourceSize = source.size();

	for (int i = 0; i < sourceSize; i++)
	{
		vector[i] += source.at(i) * scalar;
	}
}

/** Scalar product for two double vectors.
	 * @param vecA, vecB two double vectors with the same size.
	 * @return double which represents the dot product.
	 * */
double RecommenderSystem::_dotProduct(const vector<double> &vecA, const vector<double> &vecB)
{

	return std::inner_product(std::begin(vecA), std::end(vecA),
							  std::begin(vecB), 0.0);
}

/** frien function, calculating the norm of a vector.
	 * @param vec a vector of doubles.
	 * @return a double which represents the norm.
	 * */
double norm(const vector<double> &vec)
{
	double product = 0.0;

	for (auto &i : vec)
	{
		product += (i * i);
	}
	return std::sqrt(product);
}


/** This function recommends the user a film from the list according to his rankings
	 * @param userName the costumer name to recommend for
	 * @return film name on success, USER NOT FOUND on failure.
	 * */string RecommenderSystem::recommendByContent(const string &userName)
{
	if (_usersRating.find(userName) == _usersRating.end())
	{
		return UND_MSG;
	}

	std::unordered_map<string, double> userMap = _usersRating.at(userName);

	double sum = 0.0, nonNA = 0.0;
	string highestRateName;
	double highestRateScore = -INT_MAX;

	for (auto &film: _usersRating.at(userName))
	{
		if (film.second != NA_VALUE)
		{
			sum += film.second;
			nonNA++;
		}
	}

	double avg = sum / nonNA;
	vector<double> preferVector(_moviesScores.begin()->second.size(), 0);

	for (auto &userFilm:userMap)
	{
		if (userFilm.second != NA_VALUE)
		{
			userFilm.second -= avg;
			_addToPreferenceVector(_moviesScores.at(userFilm.first),
								   preferVector, userFilm.second);
		}
	}

	for (auto &singleFilm : _rankedFilmOrder)
	{
		if (_usersRating.at(userName).at(singleFilm) == NA_VALUE)
		{

			double product = _dotProduct(preferVector, _moviesScores.at(singleFilm))
							 / (norm(preferVector) * _normedMoviesVectors.at(singleFilm));

			if (highestRateScore == -INT_MAX || product > highestRateScore)
			{
				highestRateName = singleFilm;
				highestRateScore = product;
			}
		}
	}
	return highestRateName;
}

/** Friend function, Sorting pairs of string and double
	 * @param var1 a pair of string and double
	 * @param var2 a pair of string and double
	 * @return true if var1 is greater than var2, false otherwise.
	 * */
bool pairDoubleSort(const std::pair<string, double> &var1, const std::pair<string, double> &var2)
{
	return var1.second > var2.second;
}

/** This function predict a film to the user according to his rankings and the films feature
	 * @param movieName not ranked film by the user
	 * @param userName the costumer name
	 * @param k the k'th number of close ranked-by-the-user films related to movieName
	 * @return the predicted rank in double.
	 * */
double RecommenderSystem::predictMovieScoreForUser(const string &movieName,
												   const string &userName, int k)
{
	if(_moviesScores.find(movieName) == _moviesScores.end() || _usersRating.find(userName) ==
	_usersRating.end()){
		return METHOD_FAILURE;
	}

	vector<string> notRated;

	vector<std::pair<string, double>> setN;

	for (auto &singleFile:_rankedFilmOrder)
	{
		if (_usersRating.at(userName).at(singleFile) != NA_VALUE)
		{
			double val = _dotProduct(_moviesScores.at(movieName),
									 _moviesScores.at(singleFile))
						 / (_normedMoviesVectors.at(movieName)
							* _normedMoviesVectors.at(singleFile));

			std::pair<string, double> toAdd = {singleFile, val};

			setN.push_back(toAdd);
		}
	}

	std::sort(setN.begin(), setN.end(), pairDoubleSort);

	double sum = 0.0, divideSum = 0.0;

	for (int i = 0; i < k; ++i)
	{
		double r = _usersRating.at(userName).at(setN[i].first);
		sum += (setN[i].second * r);
		divideSum += setN[i].second;
	}
	return (sum / divideSum);
}


/** Recommending a not ranked film to the user.
	 * @param userName the costumer name.
	 * @return k the k'th number of close ranked-by-the-user films related to movieName.
	 * */
string RecommenderSystem::recommendByCF(const string &userName, int k)
{
	vector<string> notRankedVectors;

	if (_usersRating.find(userName) == _usersRating.end())
	{
		return UND_MSG;
	}

	for (auto &temp : _rankedFilmOrder)
	{
		if (_usersRating.at(userName).at(temp) == NA_VALUE)
		{
			notRankedVectors.push_back(temp);
		}
	}

	double maxRate = 0.0;
	string maxRateFilm;
	double tempRate;
	for (auto &itemNA:notRankedVectors)
	{
		tempRate = predictMovieScoreForUser(itemNA, userName, k);
		if (tempRate > maxRate)
		{
			maxRate = tempRate;
			maxRateFilm = itemNA;
		}
	}
	return maxRateFilm;
}

/** Reads the data of films features, rankings and users' rankings into data structures
	 * @param moviesAttributesFilePath a path to the movies features and ranks file.
	 * @param userRanksFilePath path to user's rankings file.
	 * @return 0 on success, -1 otherwise.
	 * */
int RecommenderSystem::loadData(const string &moviesAttributesFilePath,
								const string &userRanksFilePath)
{
	if (_readMoviesAttributesFile(moviesAttributesFilePath) == METHOD_FAILURE)
	{
		return METHOD_FAILURE;
	}

	if (_readUserRanksFile(userRanksFilePath) == METHOD_FAILURE)
	{
		return METHOD_FAILURE;
	}
	return METHOD_SUCCESS;
}

/** Reading the movies features file and store the films and their values into a map.
	 * @param moviesAttributesFilePath the film attributes file path.
	 * @return 0 on success, -1 otherwise.
	 * */
int RecommenderSystem::_readMoviesAttributesFile(const string &moviesAttributesFilePath)
{
	ifstream file1;
	string line;
	file1.open(moviesAttributesFilePath);
	if (!file1)
	{
		std::cerr << FILE_ERR_MSG + moviesAttributesFilePath << std::endl;
		return METHOD_FAILURE;
	}
	while (getline(file1, line))
	{

		std::istringstream iss(line);
		string filmName;
		iss >> filmName;
		_moviesScores[filmName];
		double rate;
		vector<double> ratesVector;
		while (iss >> rate)
		{
			ratesVector.push_back(rate);
		}
		_moviesScores[filmName] = ratesVector;
		_normedMoviesVectors[filmName] = norm(ratesVector);
	}
	file1.close();
	return METHOD_SUCCESS;
}

/** Reading the the user's ranking file and store the users, films and their values into a map.
	 * @param userRanksFilePath the rankings file path.
	 * @return 0 on success, -1 otherwise.
	 * */
int RecommenderSystem::_readUserRanksFile(const string &userRanksFilePath)
{
	ifstream file2;
	string line2;
	file2.open(userRanksFilePath);
	if (!file2)
	{
		std::cerr << FILE_ERR_MSG + userRanksFilePath << std::endl;
		return METHOD_FAILURE;
	}

	getline(file2, line2);

	std::istringstream iss2(line2);
	string film2;
	unordered_map<string, double> internalRate;

	while (iss2 >> film2)
	{
		internalRate[film2];
		_rankedFilmOrder.push_back(film2);
	}

	while (getline(file2, line2))
	{
		unordered_map<string, double> userS = internalRate;
		string userName;
		std::istringstream iStringStream(line2);
		iStringStream >> userName;
		_usersRating[userName];
		string userRate;
		int i = 0;
		const int rankedFilmsSize = _rankedFilmOrder.size();
		while (iStringStream >> userRate && i != rankedFilmsSize)
		{
			if (userRate == NOT_RANKED_FILM)
			{
				userS[_rankedFilmOrder.at(i)] = NA_VALUE;
				i++;
			}
			else
			{
				std::istringstream toInt(userRate);
				double r;
				toInt >> r;
				userS[_rankedFilmOrder.at(i)] = r;
				i++;
			}
		}
		_usersRating[userName] = userS;
	}
	file2.close();
	return METHOD_SUCCESS;
}
