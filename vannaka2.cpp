#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <random>
#include <cassert>

using ld = long double;

std::mt19937 rng((int) std::chrono::steady_clock::now().time_since_epoch().count());

struct Task {
    std::string name;
    int weight;
    bool canDo;
};

std::vector<Task> tasks[256];

std::pair<std::string, bool> rollTask(const std::vector<Task> &taskVector, const std::string &lastTask) {
    int totalWeight = 0;
    for(auto task : taskVector) if(task.name != lastTask) {
        totalWeight += task.weight;
        // if(task.canDo) {
        //     goodWeight += task.weight;
        // }
    }
    // std::cout << taskVector.size() << '\n';
    // std::cout << goodWeight << "/" << totalWeight << '\n';
    int roll = std::uniform_int_distribution(1, totalWeight)(rng);
    for(auto task : taskVector) if(task.name != lastTask) {
        roll -= task.weight;
        if(roll <= 0) {
            return {task.name, task.canDo};
        }
    }
    assert(0);
    return {"INVALID", false};
}

int main() {
    {
        std::string task;
        while(std::cin >> task) {
            std::cerr << "reading task " << task << std::endl;
            std::string canDo, masters;
            int points;
            assert(std::cin >> canDo);
            assert(canDo == "yes" || canDo == "no");
            assert(std::cin >> masters);
            assert(std::cin >> points);
            for(auto master : masters) {
                tasks[(int) master].push_back({task, points, canDo == "yes"});
            }
        }
    }
    std::vector<int> points(1006, 4);
    std::vector<int> pointsWild(1006, 25);
    for(int i = 10; i <= 1000; i += 10) {
        points[i] = 20;
        pointsWild[i] = 125;
        if(i % 50 == 0) {
            points[i] = 60;
            pointsWild[i] = 375;
        }
        if(i % 100 == 0) {
            points[i] = 100;
            pointsWild[i] = 625;
        }
        if(i % 250 == 0) {
            points[i] = 140;
            pointsWild[i] = 875;
        }
        if(i % 1000 == 0) {
            points[i] = 200;
            pointsWild[i] = 1250;
        }
    }
    // slayer update
    for(int i = 0; i < (int) points.size(); i++) {
        points[i] *= 2;
    }
    points[1] = points[2] = points[3] = points[4] = points[5] = 0;
    pointsWild[1] = pointsWild[2] = pointsWild[3] = pointsWild[4] = pointsWild[5] = 0;
    std::string policy(1007, 'S');
    // for(int i = 10; i < 1006; i += 10) {
    //     policy[i] = 'v';
    // }
    policy[1] = 'T';
    policy[10] = 'v';
    policy[20] = 'v';
    policy[50] = policy[100] = policy[150] = policy[200] = policy[250] = policy[300] = 'V';
    // for(int i = 301; i < 305; i++) {
    //     policy[i] = 'v';
    // }
    // for(int i = 10; i < 1006; i += 10) {
    //     policy[i] = 'v';
    // }
    for(int i = 50; i < 1006; i += 50) {
        policy[i] = 'V';
    }
    for(int i = 10; i < 1006; i += 1) {
        policy[i] = 'v';
    }
    // for(int i = 6; i < 1006; i += 1) {
    //     policy[i] = 'v';
    // }
    int success = 0;
    long long totalTasks = 0;
    long long rat = 0;
    const int startingPoints = 70;
    const int target = 10000;
    for(int simulation = 1; simulation <= 100000; simulation++) {
        long long totalBalance = startingPoints;
        long long monkeyTasks = 0;
        std::string lastTask;
        int wildStreak = 1;
        auto doKrystilia = [&]() {
            //return;
            do {
                auto nextTask = rollTask(tasks['K'], lastTask);
                lastTask = nextTask.first;
                if(!nextTask.second) {
                    wildStreak = 1;
                    break;
                } else {
                    totalTasks++;
                    totalBalance += pointsWild[wildStreak];
                    if(pointsWild[wildStreak]) {
                        //std::cout << "at streak " << i << " got " << pointsWild[i] << '\n';
                    }
                    wildStreak++;
                    if(wildStreak >= 1006) {
                        wildStreak -= 1000;
                    }
                }
            } while(wildStreak < 5);
        };
        for(int rep = 1; rep <= 100000000 && totalBalance >= 0 && totalBalance < target; rep++) {
            for(int i = 1; i <= 1000 && totalBalance >= 0 && totalBalance < target; i++) {
                if(policy[i] == 'T' || policy[i] == 'S') {
                    if(policy[i] == 'T' && i > 1 && 0) doKrystilia();
                    auto nextTask = rollTask(tasks[(int) policy[i]], lastTask);
                    lastTask = nextTask.first;
                    while(!nextTask.second) {
                        nextTask = rollTask(tasks[(int) policy[i]], lastTask);
                        monkeyTasks++;
                        totalBalance -= 30;
                        assert(nextTask.second);
                    }
                    if(totalBalance >= 0) totalTasks++;
                    if(totalBalance >= 0 && nextTask.first == "Rats") rat++;
                    lastTask = nextTask.first;
                } else if(policy[i] == 'v') {
                    auto nextTask = rollTask(tasks['V'], lastTask);
                    lastTask = nextTask.first;
                    if(!nextTask.second) {
                        break;
                    } else {
                        totalTasks++;
                        totalBalance += points[i];
                    }
                } else if(policy[i] == 'V') {
                    auto nextTask = rollTask(tasks['V'], lastTask);
                    lastTask = nextTask.first;
                    while(!nextTask.second && totalBalance >= 30) {
                        nextTask = rollTask(tasks['V'], lastTask);
                        lastTask = nextTask.first;
                        totalBalance -= 30;
                    }
                    assert(totalBalance >= 0);
                    if(!nextTask.second) {
                        break;
                    }
                    totalTasks++;
                    totalBalance += points[i];
                }
            }
            //std::cout << "at " << rep << " reps, ended with " << totalBalance << " points and got average of " << (ld) (totalBalance + (long long) 30 * monkeyTasks) / ((ld) monkeyTasks + 1e-9) << " points per monkey task" << std::endl;
        }
        if(totalBalance < 0) {
            std::cout << "simulation " << simulation << " failed" << std::endl;
        } else {
            success++;
            std::cout << "simulation " << simulation << " ended with " << totalBalance << std::endl;
        }
        std::cout << "escape rate is " << (ld) success / simulation << std::endl;
        //std::cout << "totalTasks is " << totalTasks << ", average is " << (ld) totalTasks / simulation << std::endl;
        std::cout << "rat tasks is " << rat << ", average is " << (ld) rat / simulation << std::endl;
        std::cout << (ld) totalTasks / simulation << " average tasks until target or lock" << std::endl;
    }
}
