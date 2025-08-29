#include <bits/stdc++.h>

double computeDropRate(double monsters, double rate) { return 1 - pow(1-rate, monsters); }

int main() {
    const int totalTuraelWeight = 156;
    const int monkeyWeight = 6;
    const double monkeyProbability = monkeyWeight / (double) totalTuraelWeight;
    const int caveCrawlerWeight = 8;
    const double dropRatePerCaveCrawlerTask = computeDropRate((15+30)/2.0, 1.0/(200 * 1337));
    const double averageTuraelTask = (284 * 0.6) / 60;

    const int totalVannakaWeight = 169;
    const int doVannakaWeight = 93 - 8 - 7;
    const int kalphiteWeight = 7;
    const int pyrefiendWeight = 8;
    const double dropRatePerPyrefiendTask = computeDropRate((40+90)/2.0, 1.0/(200 * 1144));
    const double averageVannakaTask = (64 * 30) / 60.0;

    const int baseVannakaPoints = 8;
    auto getMultiplier = [](int streak) {
        if(streak <= 5) return 0;
        else if(streak % 1000 == 0) return 50;
        else if(streak % 250 == 0) return 35;
        else if(streak % 100 == 0) return 25;
        else if(streak % 50 == 0) return 15;
        else if(streak % 10 == 0) return 5;
        else return 1;
    };

    const int maxStreak = 1008;
    const int maxPoints = 1000;
    const double INF = 1e6 * 60;
    std::vector<std::vector<std::pair<double, char>>> policy(maxStreak, std::vector<std::pair<double, char>>(maxPoints+1, {INF, '?'}));
    const int iterations = 100000;

    auto newStreak = [&](int streak) { return streak == maxStreak ? streak - 1000 : streak; };
    auto newPoints = [&](int points) { return std::min(maxPoints, points); };
    
    auto turaelBoost = [&](int streak, int points) {
        return std::pair<double, char>(averageTuraelTask + 6 / 60.0 /*storage cost of 6 seconds*/ +
            ((totalTuraelWeight - monkeyWeight - caveCrawlerWeight) + caveCrawlerWeight * (1 - dropRatePerCaveCrawlerTask)) / (totalTuraelWeight - monkeyWeight) *
            policy[newStreak(streak+1)][newPoints(points)].first, 'T');
    };
    auto vannakaWithPointSkipForBadTasks = [&](int streak, int points) {
        if(points < 30) return std::pair<double, char>(INF, '?');
        double skipProbability = 1.0 - (doVannakaWeight + kalphiteWeight + pyrefiendWeight) / (double) totalVannakaWeight;
        double skipCost = skipProbability * (10 / 60.0 + policy[streak][points-30].first); // assuming 10 seconds to skip
        double nextStateCost = policy[newStreak(streak+1)][newPoints(points+baseVannakaPoints*getMultiplier(streak+1))].first;
        double nonskipCost = (1 - skipProbability) * (
            averageVannakaTask +
            (doVannakaWeight + kalphiteWeight + pyrefiendWeight * (1 - dropRatePerPyrefiendTask)) / (doVannakaWeight + kalphiteWeight + pyrefiendWeight) *
            nextStateCost
        );
        return std::pair<double, char>(skipCost + nonskipCost, 'P');
    };
    auto vannakaWithPointSkipPyrefiend = [&](int streak, int points) {
        if(points < 30) return std::pair<double, char>(INF, '?');
        double skipProbability = 1.0 - pyrefiendWeight / (double) totalVannakaWeight;
        double skipCost = skipProbability * (10 / 60.0 + policy[streak][points-30].first); // assuming 10 seconds to skip
        double nextStateCost = policy[newStreak(streak+1)][newPoints(points+baseVannakaPoints*getMultiplier(streak+1))].first;
        double nonskipCost = (1 - skipProbability) * (
            averageVannakaTask / 2 + 3 + (1 - dropRatePerPyrefiendTask) * nextStateCost
        );
        return std::pair<double, char>(skipCost + nonskipCost, 'p');
    };
    auto weakVannakaTurael = [&](int streak, int points) {
        double skipProbability = 1.0 - (doVannakaWeight + kalphiteWeight + pyrefiendWeight) / (double) totalVannakaWeight;
        double skipCost = skipProbability * (
            30 / 60.0 +
            monkeyProbability * (points >= 30 ? policy[0][points-30].first : INF) + 
            (1 - monkeyProbability) * (averageTuraelTask + policy[1][points].first)
        ); // assuming 30 seconds to skip
        double nextStateCost = policy[newStreak(streak+1)][newPoints(points+baseVannakaPoints*getMultiplier(streak+1))].first;
        double nonskipCost = (1 - skipProbability) * (
            averageVannakaTask +
            (doVannakaWeight + kalphiteWeight + pyrefiendWeight * (1 - dropRatePerPyrefiendTask)) / (doVannakaWeight + kalphiteWeight + pyrefiendWeight) *
            nextStateCost
        );
        return std::pair<double, char>(skipCost + nonskipCost, 'M');
    };
    auto mediumVannakaTurael = [&](int streak, int points) {
        double skipProbability = 1.0 - (kalphiteWeight + pyrefiendWeight) / (double) totalVannakaWeight;
        double skipCost = skipProbability * (
            30 / 60.0 +
            monkeyProbability * (points >= 30 ? policy[0][points-30].first : INF) + 
            (1 - monkeyProbability) * (averageTuraelTask + policy[1][points].first)
        ); // assuming 30 seconds to skip
        double nextStateCost = policy[newStreak(streak+1)][newPoints(points+baseVannakaPoints*getMultiplier(streak+1))].first;
        double nonskipCost = (1 - skipProbability) * (
            averageVannakaTask * 3 / 4 +
            (kalphiteWeight + pyrefiendWeight * (1 - dropRatePerPyrefiendTask)) / (kalphiteWeight + pyrefiendWeight) *
            nextStateCost
        );
        return std::pair<double, char>(skipCost + nonskipCost, 'V');
    };
    auto aggressiveVannakaTurael = [&](int streak, int points) {
        if(points < 30) return std::pair<double, char>(INF, '?');
        double skipProbability = 1.0 - (kalphiteWeight + pyrefiendWeight) / (double) totalVannakaWeight;
        double skipCost = skipProbability * (
            30 / 60.0 +
            monkeyProbability * (points >= 30 ? policy[0][points-30].first : INF) + 
            (1 - monkeyProbability) * (averageTuraelTask + policy[1][points].first)
        ); // assuming 30 seconds to skip
        double nextStateCost = policy[newStreak(streak+1)][newPoints(points+baseVannakaPoints*getMultiplier(streak+1))].first;
        double nonskipCost = (1 - skipProbability) * (
            averageVannakaTask / 2 + 3 +
            (kalphiteWeight * policy[streak][points - 30].first + pyrefiendWeight * (1 - dropRatePerPyrefiendTask) * nextStateCost) / (kalphiteWeight + pyrefiendWeight)
        );
        return std::pair<double, char>(skipCost + nonskipCost, 'v');
    };
    std::cerr << std::fixed << std::setprecision(10);
    for(int rep = 0; rep < iterations; rep++) {
        for(int i = 0; i < maxStreak; i++) for(int j = 0; j <= maxPoints; j++) {
            policy[i][j] = std::min({turaelBoost(i, j), vannakaWithPointSkipForBadTasks(i, j), vannakaWithPointSkipPyrefiend(i, j), weakVannakaTurael(i, j), mediumVannakaTurael(i, j), aggressiveVannakaTurael(i, j)});
        }
        for(int i = maxStreak-1; i >= 0; i--) for(int j = maxPoints; j >= 0; j--) {
            policy[i][j] = std::min({turaelBoost(i, j), vannakaWithPointSkipForBadTasks(i, j), vannakaWithPointSkipPyrefiend(i, j), weakVannakaTurael(i, j), mediumVannakaTurael(i, j), aggressiveVannakaTurael(i, j)});
        }
        std::cerr << "after " << rep << " iterations got " << policy[0][500].first / 60 << "h" << std::endl;
    }
    for(int i = 0; i < maxStreak; i++) for(int j = 0; j <= maxPoints; j++) {
        std::cout << policy[i][j].second;
        if(j == maxPoints) std::cout << std::endl;
    }
}
