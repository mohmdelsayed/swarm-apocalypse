#      ___           ___            ___                  ___           ___           ___     
#     /\__\         /|  |          /\  \                /\__\         /\__\         /\  \    
#    /:/ _/_       |:|  |         /::\  \              /:/ _/_       /:/ _/_        \:\  \   
#   /:/ /\__\      |:|  |        /:/\:\__\            /:/ /\  \     /:/ /\__\        \:\  \  
#  /:/ /:/ _/_   __|:|__|       /:/ /:/  /           /:/ /::\  \   /:/ /:/ _/_   _____\:\  \ 
# /:/_/:/ /\__\ /::::\__\_____ /:/_/:/  /           /:/__\/\:\__\ /:/_/:/ /\__\ /::::::::\__\
# \:\/:/ /:/  / ~~~~\::::/___/ \:\/:/  /            \:\  \ /:/  / \:\/:/ /:/  / \:\~~\~~\/__/
#  \::/_/:/  /      |:|~~|      \::/__/              \:\  /:/  /   \::/_/:/  /   \:\  \      
#   \:\/:/  /       |:|  |       \:\  \               \:\/:/  /     \:\/:/  /     \:\  \     
#    \::/  /        |:|__|        \:\__\               \::/  /       \::/  /       \:\__\    
#     \/__/         |/__/          \/__/                \/__/         \/__/         \/__/    

import os
import xml.etree.ElementTree as et 
import shutil

######################################################
#############     HOW TO USE         #################
######################################################
# 1- Change the range in the for loop. Example -> for i in range (1,100)
# 2- Make the variable you want to sweep equal to the counter i
# 3- Change the output name of the folder generated (generated_experiments_MYPARAM_altered)

#get the path where we are now
base_path = os.path.dirname(os.path.realpath(__file__))
# print (base_path)

#select new folder name each time you run the generator script to avoid conffilects
my_folder_name = "generated_experiments_MYPARAM_altered"

#make new folder for the generated experiments 
folder_path = os.path.join(base_path, my_folder_name)
os.mkdir(folder_path)

#Get our main experiment xml template path
xml_path = os.path.join(base_path,"eyebot_apocalypse")

#get templet name
base_file_name = xml_path

#number of generated experiments is 10

number_of_experiments = 11

for i in range(number_of_experiments):

    ################################# USER defined Parameter ######################################
    #simulation_parameters to be write to the template when generating the experiments 
    #Add your parameters here and factorize it with *i to be changed in each generated experiment
    #<apocalypse></>
    alpha_healthy = 1           #  1  is optimum   
    beta_healthy = 0.1          # 0.1 is optimum
    gamma_healthy = 0.1         # 0.1 is optimum
    delta_healthy = 0.1         # 0.1 is optimum
    alpha_infected = 0.1        # 0.1 is optimum
    beta_infected = 0.1         # 0.1 is optimum
    gamma_infected = 1          #  1  is optimum
    delta_infected = 0.1        # 0.1 is optimum
    InfectionPercentage = 0.2   # 0.2 is optimum
    InfectionStart = 50         # 50  is optimum
    InfectionTerminal = 600     # 600 is optimum
    InfectionDistance = 200     # 200 is optimum
    CuringDistance = 500        # 500 is optimum
    CuringTime = 33             # 33  is optimum
    visualize = 0               #  1  to show counter 
    log = 0               #  1  to log messages 
    #<flocking></>
    target_distance = 250       #  250  is optimum  
    AttractionFactor= 0.9       #  0.9  is optimum  
    exponent = 2                #  2    is optimum  
    GoalGain=0.030              # 0.030 is optimum                  
    ########################################Don't MODIFY ###########################################
    #simulation_parameters to be write to the template 
    apocalypse_simulation_parameters ={  "alpha_healthy" : str(alpha_healthy),
                                         "beta_healthy" : str(beta_healthy),
                                         "gamma_healthy" : str(gamma_healthy),
                                         "delta_healthy" : str(delta_healthy),
                                         "alpha_infected" : str(alpha_infected),
                                         "beta_infected" : str(beta_infected),
                                         "gamma_infected" : str(gamma_infected),
                                         "delta_infected" : str(delta_infected),
                                         "InfectionPercentage" : str(InfectionPercentage),
                                         "InfectionStart" : str(InfectionStart),
                                         "InfectionTerminal" : str(InfectionTerminal),
                                         "InfectionDistance" : str(InfectionDistance),
                                         "CuringDistance" : str(CuringDistance),
                                         "CuringTime" : str(CuringTime),
                                         "log" : str(log),
                                         "visualize" : str(visualize) }

    flocking_simulation_parameters ={    "target_distance" : str(target_distance),
                                         "AttractionFactor" : str(AttractionFactor),
                                         "exponent": str(exponent),
                                         "GoalGain":str(GoalGain)}
    # Initial new name
    new_file_name = os.path.join(folder_path,'eyebot_apocalypse_ex'+str(i)+'.argos')
    shutil.copy2(xml_path, new_file_name)
    
    #parse the xml file and handel its root tag
    tree = et.parse(new_file_name)
    root = tree.getroot()

    #Accessing the location where we should insert our parameters in our template
    param_tag = root[1][0][2]

    ###########################################
    # To find the list of parameters in any_tag
    # any_tag = param_tag[0]
    # list_of_any_tag_param = []
    # list_of_any_param = any_tag.keys()
    # print(list_of_any_tag_param)
    ########################################### 
    
    #fill the <param></param> tag
    # et.SubElement(param_tag,'apocalypse').set(simulation_parameters)
    et.SubElement(param_tag,'apocalypse',attrib=apocalypse_simulation_parameters)
    et.SubElement(param_tag,'flocking',attrib=flocking_simulation_parameters)

    #Write all to the same new modifed templet of experiment
    tree.write(new_file_name)

print("Experiments Generated Successfully! ")