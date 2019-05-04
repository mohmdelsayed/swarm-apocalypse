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
# 2- go to the experiments_generator function and change the parameters
# 2- Make the variable you want to sweep over the generated experiments equal to the counter i
# 3- Change the output name of the folder generated (generated_experiments_MYPARAM_altered)
# 5- Change the output name of the folder results (generated_experiments_results)


#number of generated experiments we want
number_of_experiments = 11

#select new folder name each time you run the generator script to avoid conffilects

#experiments generated folder name
my_experiments_folder_name = "generated_experiments_MYPARAM_altered"
#results from the generated experiments folder name
my_result_folder_name = "generated_experiments_results"


def argos_experiment_templete_locator():

    global base_path
    global base_file_name
    global folder_path
    #get the path where we are now
    base_path = os.path.dirname(os.path.realpath(__file__))
    # print (base_path)

    #make new folder for the generated experiments 
    folder_path = os.path.join(base_path, my_experiments_folder_name)
    os.mkdir(folder_path)

    #Get our main experiment xml template path
    xml_path = os.path.join(base_path,"eyebot_apocalypse")

    #get templet name
    base_file_name = xml_path


def shell_scripts_generator():

    global script_run
    global script_kill
    global script_delete

    #call the template locator
    argos_experiment_templete_locator()
    #set path to swarm apocalypse main folder
    script_path = os.path.dirname(base_path)
    
    #generate script to run all experiments simultaneously 
    script_run_name = os.path.join(script_path,'run_all_experiments.sh')
    # f =open("run_all_experiment.sh","w+")
    script_run =open(script_run_name,"w+")
    script_run.write("#!/bin/bash \r\n\n"+"mkdir -p generated_experiments_results \r\n\n" )

    #generate script to kill all experiments simultaneously 
    script_kill_name = os.path.join(script_path,'kill_all_experiments.sh')
    script_kill =open(script_kill_name,"w+")
    script_kill.write("#!/bin/bash \r\n\n"+"kill -9 `pgrep argos3` \r\n\n"+"echo \"All Experiments running in background  is killd! \" \r\n\n")

    #generate script to delete all experiments and its results 
    script_delete_name = os.path.join(script_path,'delete_all_experiments.sh')
    script_delete =open(script_delete_name,"w+")
    script_delete.write("#!/bin/bash \r\n\n"+"rm -rf experiments/"+ my_experiments_folder_name +"\r\n\n"+"rm -rf "+my_result_folder_name+"\r\n\n"+"sudo rm -rf delete_all_experiments.sh \r\n\n"+"sudo rm -R -f run_all_experiments.sh \r\n\n"+"sudo rm -R -f delete_all_experiments.sh \r\n\n"+"sudo rm -R -f kill_all_experiments.sh \r\n\n"+"echo \"All Experiments files and folders you generated is deleted! \" \r\n\n" )


def experiments_generator():

    #call the shell_scripts_generator
    shell_scripts_generator()

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
        InfectionDistance = 200+40*i     # 200 is optimum
        CuringDistance = 300       # 500 is optimum
        CuringTime = 33             # 33  is optimum
        visualize = 0               #  100  to show counter 
        log = 0                     #  1  to log messages 
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
        shutil.copy2(base_file_name, new_file_name)

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
        
        script_run.write("nohup argos3 -c experiments/"+my_experiments_folder_name+"/eyebot_apocalypse_ex"+str(i)+".argos > "+my_result_folder_name+"/eyebot_apocalypse_ex"+str(i)+".out & \r\n")

    script_run.close
    script_kill.close
    script_delete.close


if __name__== "__main__":
  experiments_generator()
  print("Experiments Generated Successfully! ")

