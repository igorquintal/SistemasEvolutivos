from random import randint, uniform, randrange
from operator import itemgetter
import subprocess
import time
import sys

RobotGenes = ['pixels_analisados', 'angulo_visao_dir', 'angulo_visao_esq', 'n_giro', 'vel_max', 'delta_vel_linear', 'repulsao_limite', 'peso', 'incremento_laser']
PopulationSize = 3
BestRobotsOfEachGeneration =[]
BestFitness = 0

#The dawn of Robots
def CreateRobotsFromFile(FileName):

	readFile = open(FileName)
	dataSet = readFile.readlines()

	Robot = {}
	RobotsList = []

	for line in dataSet:

		if line.split('=')[0] == 'incremento_laser':
			Robot[line.split('=')[0]] = float(line.split('=')[1][:-1])
			RobotsList.append(Robot.copy())
			Robot.clear()
		else:
			Robot[line.split('=')[0]] = float(line.split('=')[1][:-1])

	readFile.close()

	return RobotsList

#A couple of robots is having sex!!
def CreateChildRobot(Parent1, Parent2):

	BabyRobot = {}

	for gene in RobotGenes:

		chance = randint(1,100)

		if(chance>50):
			BabyRobot[gene] = Parent1[gene]
		else:
			BabyRobot[gene] = Parent2[gene]

	return BabyRobot
	

#Robots start falling in love with each other!
def Crossover(RobotsList):

	NewPopulationList = []
	ChildRobotsList = []

	while(len(RobotsList) > 1):
		
		#Selecting the parents!!
		Parent1_Index = randint(0, len(RobotsList)-1)
		Parent1 = RobotsList[Parent1_Index]
		RobotsList.remove(Parent1)

		Parent2_Index = randint(0, len(RobotsList)-1)
		Parent2 = RobotsList[Parent2_Index]
		RobotsList.remove(Parent2)
		
		#A baby is on the way!!
		BabyRobot = CreateChildRobot(Parent1, Parent2)

		Mutation(BabyRobot)

		ChildRobotsList.append(BabyRobot)
		NewPopulationList.append(Parent1)
		NewPopulationList.append(Parent2)


	NewPopulationList = NewPopulationList + ChildRobotsList + RobotsList

	return NewPopulationList

#Creates the mutation on robots!!
def Mutation(Robot):

	MutationChance = randint(0,9)
	if(MutationChance > 5):
		#valor entre 5 e 10
		valor= randint(-1,1)
		if ((valor+Robot['pixels_analisados']>=5) and (valor+Robot['pixels_analisados']<=10)): 
			Robot['pixels_analisados'] += valor

	MutationChance = randint(0,9)
	if(MutationChance < 2):
		#valor entre 40 e 60
		valor= randint(-5,5)
		if ((valor+Robot['angulo_visao_dir']>=40) and (valor+Robot['angulo_visao_dir']<=60)):
			Robot['angulo_visao_dir'] += valor

	MutationChance = randint(0,9)
	if(MutationChance < 2):
		#valor entre 300 e 320
		valor= randint(-5,5)
		if ((valor+Robot['angulo_visao_esq']>=300) and (valor+Robot['angulo_visao_esq']<=320)):
			Robot['angulo_visao_esq'] += valor
	
	MutationChance = randint(0,9)
	if(MutationChance < 2):
		#valor entre 30 e 120
		valor= randint(-3,3)
		if ((valor+Robot['n_giro']>=3) and (valor+Robot['n_giro']<=60)):
			Robot['n_giro'] += valor

	MutationChance  = randint(0,9)
	if(MutationChance < 6):
		#valor entre 0.20 e 2.00
		valor= round(uniform(-0.3,0.3), 4)
		if ((valor+Robot['vel_max']>=0.2) and (valor+Robot['vel_max']<=3.0)):
			Robot['vel_max'] += valor

	MutationChance  = randint(0,9)
	if(MutationChance < 6):
		#valor entre 0.02 e 0.1
		valor= round(uniform(-0.1,0.1), 4)
		if ((valor+Robot['delta_vel_linear']>=0.02) and (valor+Robot['delta_vel_linear']<=0.1)):
			Robot['delta_vel_linear'] += valor
	
	MutationChance  = randint(0,9)
	if(MutationChance < 5):
		#valor entre 2.00 e 1.30
		valor= round(uniform(-0.2,0.2), 4)
		if ((valor+Robot['repulsao_limite']>=1.0) and (valor+Robot['repulsao_limite']<=3.0)):
			Robot['repulsao_limite'] += valor
	
	MutationChance  = randint(0,9)
	if(MutationChance <5):
		#valor entre 0.10 e 1.00
		valor= round(uniform(-0.1,0.1), 4)
		if ((valor+Robot['peso']>=0.1) and (valor+Robot['peso']<=1.0)):
			Robot['peso'] += valor

	MutationChance  = randint(0,9)
	if(MutationChance <2):
		#valor entre 10 20 e 30
		valor= randrange(-10, 10, 10)
		if ((valor+Robot['incremento_laser']>=10) and (valor+Robot['incremento_laser']<=30)):
			Robot['incremento_laser'] += valor

#LET THE GAMES BEGIN!!
def FitnessCalculation(RobotsList):

	for Robot in RobotsList:

		if 'fitness' not in Robot:

			#print('FITNESS TEM TANTOS GENES ' + str(len(Robot)))

			MapProcess = subprocess.Popen(['player', '/home/suporte/teste/prm_1/mapa/mapa1.cfg'], shell=False)
			time.sleep(1)
			RobotFitness = subprocess.check_output(['./cvtest'] + [str(number) for number in Robot.values()], shell=False)
			
			time.sleep(1)
			
			MapProcess.kill()

			Robot['fitness'] = RobotFitness

#Chose which Robots may survive!!
def CreateNewPopulation(RobotsList):

	RobotsList.sort(key=itemgetter('fitness'), reverse=True)

	del RobotsList[PopulationSize:]
	
	#time.sleep(20)

	#Adds the robot with the best fitness to the list
	global BestRobotsOfEachGeneration
	BestRobotsOfEachGeneration.append(RobotsList[0])

	#Adds the best fitness to decide when to stop
	global BestFitness
	BestFitness = RobotsList[0]['fitness']

#Write the results to a file
def WriteResultsToFile():

	writeFile = open('myfile','w')

	index = 0
	
	for Robot in BestRobotsOfEachGeneration:

		writeFile.write('Generation {} best fitness is {} \n'.format(index, Robot['fitness']))
		index+=1

	writeFile.write('The best robot is:\n')

	for gene in RobotGenes:
		writeFile.write('{} \n'.format(BestRobotsOfEachGeneration[len(BestRobotsOfEachGeneration)-1][gene]))

	writeFile.write('{} \n'.format(BestRobotsOfEachGeneration[len(BestRobotsOfEachGeneration)-1]['fitness']))

	writeFile.close()


#Defines the main function
def main():

	FileName = sys.argv[1]
	
	RobotsList = CreateRobotsFromFile(FileName)

	cont = 0

	while (cont<40):

		RobotsList = Crossover(RobotsList)
		
		FitnessCalculation(RobotsList)

		CreateNewPopulation(RobotsList)

		cont+=1

	WriteResultsToFile()

if __name__ == "__main__":
    main()
