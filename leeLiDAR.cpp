#include <ros/ros.h>
#include <geometry_msgs/Pose2D.h>
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/Float32.h>
#include <stdlib.h> 
#include <iostream>
#include <math.h>
#include <limits>

//variables LiDAR
double deltaangulo, angulomin;
double inf = std::numeric_limits<double>::infinity();
bool flag1, flag2;
std::vector<float> datos;

// callback para leer distancia del obstaculo
void poseMessageReceived (const sensor_msgs::LaserScan& msg1){
	//Angulo en radianes
	deltaangulo=msg1.angle_increment;
	angulomin=msg1.angle_min;
	datos=msg1.ranges;
	flag2=true;
}

int main (int argc, char **argv){
	double  distancia, angulo, deltaT;
	ros::Time time1, time2;
	int cont;
	// Inicializa ROS system y crea un nodo.
	ros::init(argc, argv, "car_controller");
	ros::NodeHandle nh ;
	// Crea un objeto publicador .
	ros::Publisher pubS=nh.advertise<std_msgs::Float32>("/AutoNOMOS_mini_1/manual_control/steering",1000);
	// Crea un objeto suscriptor
	ros::Subscriber subP = nh.subscribe("/AutoNOMOS_mini_1/laser_scan", 1000, &poseMessageReceived);
	//Crea el objeto mensaje

	// Ciclo a hz Hz
	ros::Rate rate (10);

	time1  =ros::Time::now();
	time2  =ros::Time::now();

	//Ciclo principal
    	while (ros::ok()) {
    	//Procesar datos del LiDAR
		distancia=0;
		angulo=0;
		cont=0;
		flag1=false;

		if (flag2){
			//Obtener tiempo trascurrido entre mediciones
			time2  =ros::Time::now();
			deltaT=time2.toSec()-time1.toSec();
			time1=time2;

			//Recorrer el vector de distancias
			for(int i=0; i<360; i++){
				if(datos[i]>0.2 && datos[i]<inf){
					distancia=distancia+datos[i];
					cont++;
					if(i<30){
						flag1=true;
					}
					if(i>330 && flag1){
						angulo=angulo+(i*deltaangulo-2*3.141592);
					}
					else{
						angulo=angulo+i*deltaangulo;
					}
				}
			}
			//Verificar si se detecto obstaculo
			if(cont>0){
				distancia=distancia/cont;
				angulo=angulo/cont;
			}
			ROS_INFO_STREAM("angulo: "<<angulo<<" distancia: "<<distancia << " DeltaT: " << deltaT);
			flag2=false;

			//Filtro de Kalman

			
			
		}


		
		ros::spinOnce();
		// Espera hasta que es tiempo de la siguiente iteracion
		rate.sleep();
	}	
	
}
