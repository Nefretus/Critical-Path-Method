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

void load_data_matrix(int*** matrix, std::unordered_map<int, Node>& tasks) {
	std::fstream file("data80.txt", std::ios_base::in);
	int n_tasks, n_connections, execution_time, dependency, count = 0, task_nr = 0;
	file >> n_tasks >> n_connections;
	*matrix = new int* [n_connections];
	for (int i = 0; i < n_connections; i++)
		(*matrix)[i] = new int[n_connections];
	//wiersze skad, kolumny dokad
	for (int i = 0; i < n_connections; i++) {
		for (int j = 0; j < n_connections; j++) {
			(*matrix)[i][j] = 0;
		}
	}
	while (task_nr++ < n_tasks && file >> execution_time)
		tasks[task_nr] = Node { execution_time };
	while (count++ < n_connections && file >> dependency >> task_nr)
		(*matrix)[dependency - 1][task_nr - 1] = 1;
}

void load_data_list(std::unordered_map<int, Node>& tasks, std::unordered_map<int, Node>& last_tasks) {
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

void print_results(std::unordered_map<int, Node>& tasks, std::vector<int>& critical_path) {
	for (int i = 1; i < tasks.size() + 1; i++) {
		std::cout << tasks[i].es << ' ' << tasks[i].ef << ' '
			<< tasks[i].ls << ' ' << tasks[i].lf << std::endl;
	}
	std::cout << std::endl << "Critical Path" << std::endl;
	std::sort(critical_path.begin(), critical_path.end(), [&](int a, int b) { return tasks[a].ef < tasks[b].ef; });
	for (int i = 0; i < critical_path.size(); i++) {
		std::cout << critical_path[i] << ' ' << tasks[critical_path[i]].es << ' ' << tasks[critical_path[i]].ef << std::endl;
	}
}

void solve_matrix() {
	int** matrix;
	std::unordered_map<int, Node> tasks;
	load_data_matrix(&matrix, tasks);
	for (int k = 0; k < tasks.size(); k++) {
		for (int i = 0; i < tasks.size(); i++) {
			for (int j = 0; j < tasks.size(); j++) {
				if (matrix[j][i])
					tasks[i + 1].es = std::max(tasks[i + 1].es, tasks[j + 1].ef);
			}
			tasks[i + 1].ef = tasks[i + 1].es + tasks[i + 1].execution_time;
		}
	}
	std::vector<std::pair<int, Node>> tasks_sorted(tasks.begin(), tasks.end());
	std::sort(tasks_sorted.begin(), tasks_sorted.end(), [](auto a, auto b) { return a.second.ef > b.second.ef; });
	std::vector<int> crital_path;
	for (auto& [nr, task] : tasks_sorted) {
		bool last_task = true;
		for (int i = 0; i < tasks_sorted.size(); i++) {
			if (matrix[nr - 1][i]) {
				last_task = false;
				break;
			}
		}
		if (last_task) {
			tasks[nr].lf = (std::begin(tasks_sorted))->second.ef;
			tasks[nr].ls = tasks[nr].lf - tasks[nr].execution_time;
		}
		else {
			tasks[nr].lf = INT_MAX;
			tasks[nr].ls = INT_MAX;
		}
		for (int j = 0; j < tasks.size(); j++) {
			if (matrix[nr - 1][j])
				tasks[nr].lf = std::min(tasks[nr].lf, tasks[j + 1].ls);
		}
		tasks[nr].ls = tasks[nr].lf - tasks[nr].execution_time;
		if (tasks[nr].lf - tasks[nr].ef == 0)
			crital_path.push_back(nr);
	}
	print_results(tasks, crital_path);
	for (int i = 0; i < tasks.size(); i++)
		delete[] matrix[i];
	delete matrix;
}

void solve_list() {
	// list method
	std::unordered_map<int, Node> tasks, last_tasks;
	std::vector<int> crital_path;
	load_data_list(tasks, last_tasks);
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
	print_results(tasks, crital_path);
}

int main(int argc, char** argv) {
	solve_matrix();
	//solve_list();
	return 0;
}

