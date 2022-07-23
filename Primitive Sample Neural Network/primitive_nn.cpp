#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

using namespace std;
const double learning_rate = 0.05;

class Node {
	public:
		virtual double getResult() = 0;
		virtual char getType() = 0;
};

struct Link {
	Node* node;
	double coefficient;
};

class Input:public Node {
	double value;
	
	public:
		Input() {
			value = 0;
		}
	
		char getType() {
			return 'i';
		};
	
		void setValue(double value) {
			this->value = value;
		}
		
		double getResult() {
			return value;
		}
};

class Neuron:public Node {
	public:
		vector<Link> links;
		
		char getType() {
			return 'n';
		};
		
		void addParent(Node* node, double coefficient) {
			Link t = {node, coefficient};
			links.push_back(t);
		}
		
		double activate(double value) {
			return 1.0 / (1 + pow(M_E, -value));
		}
		
		double activateDerivative(double value) {
			double sig = value;
			return sig * (1 - sig);
		}
		
		void backProp(double error) {
			double weights_delta = error * activateDerivative(getResult());
			for (int i = 0; i < links.size(); ++ i)
				links[i].coefficient -= links[i].node->getResult() * weights_delta * learning_rate;
			for (int i = 0; i < links.size(); ++ i)
				if (links[i].node->getType() == 'n')
					((Neuron*)(links[i].node))->backProp(links[i].coefficient * weights_delta);
		}
		
		double getResult() {
			double result = 0;
			for (int i = 0; i < links.size(); ++ i)
				result += links[i].node->getResult() * links[i].coefficient;
			return activate(result);
		}
};

class Layer {
	public:
		Neuron* neurons;

		Layer() {}
	
		Layer(int neurons_count) {
			neurons = new Neuron[neurons_count];
		}
};

double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

class Configuration {
	Layer* hidden_layers;
	
	public:
		Input* inputs;
		Neuron* outputs;
		vector<int> *hidden_layers_conf;
		int inputs_count, outputs_count;

		Configuration(int inputs_count, vector<int> *layers_conf, int outputs_count) {
			this->inputs_count = inputs_count;
			this->outputs_count = outputs_count;
			this->hidden_layers_conf = layers_conf;
			int hlayers_size = this->hidden_layers_conf->size();
			
			inputs = new Input[inputs_count];
			hidden_layers = new Layer[hlayers_size];
			for (int i = 0; i < hlayers_size; ++ i)
				hidden_layers[i] = Layer((*layers_conf)[i]);
			outputs = new Neuron[outputs_count];
			
			// Linking first hidden layer with inputs
			for (int i = 0; i < (*layers_conf)[0]; ++ i)
				for (int j = 0; j < inputs_count; ++ j)
					hidden_layers[0].neurons[i].addParent(inputs + j, fRand(0, 1));
			
			// Linking hidden layers with each other
			for (int k = 1; k < hlayers_size; ++ k)
				for (int i = 0; i < (*layers_conf)[k]; ++ i)
					for (int j = 0; j < (*layers_conf)[k - 1]; ++ j)
						hidden_layers[k].neurons[i].addParent(hidden_layers[k - 1].neurons + j, fRand(0, 1));
			
			// Linking last hidden layer with outputs
			for (int i = 0; i < outputs_count; ++ i)
				for (int j = 0; j < (*layers_conf)[hlayers_size - 1]; ++ j)
					outputs[i].addParent(hidden_layers[hlayers_size - 1].neurons + j, fRand(0, 1));
		}
		
		void save(char* fname) {
			FILE* fp = fopen(fname, "w");
			// Saving weights from last hidden layer to outputs
			for (int i = 0; i < outputs_count; ++ i)
				for (int j = 0; j < outputs[i].links.size(); ++ j)
					fprintf(fp, "%lf\n", outputs[i].links[j].coefficient);
			
			// Saving weights from hidden layers to hidden layers/inputs
			for (int i = 0; i < hidden_layers_conf->size(); ++ i)
				for (int j = 0; j < (*hidden_layers_conf)[i]; ++ j)
					for (int k = 0; k < hidden_layers[i].neurons[j].links.size(); ++ k)
						fprintf(fp, "%lf\n", hidden_layers[i].neurons[j].links[k].coefficient);
			fclose(fp);
			printf("Successfully saved configuration to file: %s\n", fname);
		}
		
		void load(char* fname) {
			FILE* fp = fopen(fname, "r");
			// Saving weights from last hidden layer to outputs
			for (int i = 0; i < outputs_count; ++ i)
				for (int j = 0; j < outputs[i].links.size(); ++ j)
					fscanf(fp, "%lf\n", &(outputs[i].links[j].coefficient));
			
			// Saving weights from hidden layers to hidden layers/inputs
			for (int i = 0; i < hidden_layers_conf->size(); ++ i)
				for (int j = 0; j < (*hidden_layers_conf)[i]; ++ j)
					for (int k = 0; k < hidden_layers[i].neurons[j].links.size(); ++ k)
						fscanf(fp, "%lf\n", &(hidden_layers[i].neurons[j].links[k].coefficient));
			fclose(fp);
		}
		
		void printNode(Node* node) {
			printf("%.3f", node->getResult());
			if (node->getType() == 'n' && 0) {
				cout << "(";
				Neuron* tmp = (Neuron*)(node);
				for (int i = 0; i < (tmp->links).size(); ++ i) {
					printf("%.3f", (tmp->links)[i].coefficient);
					if (i < (tmp->links).size() - 1)
						cout << ", ";
				}
				cout << ")";
			}
			cout << "   ";
		}
		
		void print() {
			printf("===== Configuration =====\n\n");
			for (int i = 0; i < inputs_count; ++ i)
				printNode(inputs + i);
			printf("\n\n");
			
			for (int i = 0; i < hidden_layers_conf->size(); ++ i) {
				for (int j = 0; j < (*hidden_layers_conf)[i]; ++ j)
					printNode(hidden_layers[i].neurons + j);
				cout << endl;
			}
			printf("\n");	
			
			for (int i = 0; i < outputs_count; ++ i)
				printNode(outputs + i);
			printf("\n\n=========================\n\n");
		}
		
		void setInputs(double* inputs) {
			for (int i = 0; i < inputs_count; ++ i)
				this->inputs[i].setValue(inputs[i]);
		}
};

Configuration* getConfiguration() {
	vector<int> *hidden_layers = new vector<int>();
	(*hidden_layers).push_back(3);
	return new Configuration(3, hidden_layers, 3);
}

void initRand() {
	srand(time(NULL));
	rand();
}

int main() {
	// As an example Neural network should output mirrored output
	// Config:
	// 3 Inputs
	// 1 hidden layer with 3 neurons
	// 3 outputs
	// Learning rate: 1.0
	Configuration* nn = getConfiguration();
	nn->load("nn_conf.txt");
	double inputs[3];
	for (int i = 0; i < 3; ++ i)
		scanf("%lf", inputs + i);
	nn->setInputs(inputs);
	for (int i = 0; i < 3; ++ i)
		cout << nn->outputs[i].getResult() << " ";
	return 0;
}
