#include<iostream>
#include<fstream>
#include<vector>
#include<unordered_map>
#include<algorithm>

struct Node {
	int execution_time;
	int ls, lf, es, ef;
	std::vector<int> dependencies;
};

void load_data(std::unordered_map<int, Node>& tasks, std::unordered_map<int, Node>& last_tasks) {
	std::fstream file("data80.txt", std::ios_base::in);
	int n_tasks, n_connections, execution_time, dependency, count = 0, task_nr = 0;
	file >> n_tasks >> n_connections;
	while (task_nr++ < n_tasks && file >> execution_time) {
		tasks[task_nr] = Node { execution_time };
		last_tasks[task_nr] = Node { execution_time };
	}
	while (count++ < n_connections && file >> dependency >> task_nr) {
		tasks[task_nr].dependencies.push_back(dependency);
		if (last_tasks.find(dependency) != last_tasks.end())
			last_tasks.erase(dependency);
	}
}

int main(int argc, char** argv) {
	std::unordered_map<int, Node> tasks, last_tasks;
	std::vector<int> crital_path;
	load_data(tasks, last_tasks);
	for (auto& [nr, task] : tasks) {
		if (task.dependencies.empty()) {
			task.es = 0;
			task.ef = task.execution_time;
		}
		else {
			for (auto& [nr, task] : tasks) {
				auto dependency = std::max_element(task.dependencies.begin(), task.dependencies.end(), [&](const int a, const int b) {
					return tasks[a].ef < tasks[b].ef;
				});
				if (dependency != task.dependencies.end()) {
					task.es = tasks[*dependency].ef;
					task.ef = task.es + task.execution_time;
				}
			}
		}
	}
	std::vector<std::pair<int, Node>> tasks_sorted(tasks.begin(), tasks.end());
	std::sort(tasks_sorted.begin(), tasks_sorted.end(), [](auto a, auto b) { return a.second.ef > b.second.ef; });
	for (auto& [nr, task] : tasks_sorted) {
		if (last_tasks.find(nr) != last_tasks.end()) {
			tasks[nr].lf = (std::begin(tasks_sorted))->second.ef;
			tasks[nr].ls = tasks[nr].lf - tasks[nr].execution_time;
		}
		for (auto& dependency : task.dependencies) {
			if (tasks[dependency].lf > tasks[nr].ls || (tasks[dependency].ls == 0 && tasks[dependency].lf == 0)) {
				tasks[dependency].lf = tasks[nr].ls;
				tasks[dependency].ls = tasks[dependency].lf - tasks[dependency].execution_time;
			}
		}
		if (tasks[nr].lf - tasks[nr].ef == 0)
			crital_path.push_back(nr);
	}
	for (int i = 1; i < tasks.size() + 1; i++) {
		std::cout << tasks[i].es << ' ' << tasks[i].ef << ' '
			      << tasks[i].ls << ' ' << tasks[i].lf << std::endl;
	}
	std::cout << std::endl << "Critical Path" << std::endl;
	std::sort(crital_path.begin(), crital_path.end(), [&](int a, int b) { return tasks[a].ef < tasks[b].ef; });
	for (int i = 0; i < crital_path.size(); i++) {
		std::cout << crital_path[i] << ' ' << tasks[crital_path[i]].es << ' ' << tasks[crital_path[i]].ef << std::endl;
	}
	return 0;
}
