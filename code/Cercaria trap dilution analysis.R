library(Hmisc)
library(lme4)

lipidData<-read.csv(file.choose(),header=TRUE)
lipidData<-subset(lipidData,NumPenetrate!="NA")

percent1<-subset(lipidData,gPerL==1)
percent3<-subset(lipidData,gPerL==.333)
percent9<-subset(lipidData,gPerL==.111)
percent27<-subset(lipidData,gPerL==.037)

mean1<-mean(percent1$logPenetrate,na.action=na.omit)
mean3<-mean(percent3$logPenetrate,na.action=na.omit)
mean9<-mean(percent9$logPenetrate,na.action=na.omit)
mean27<-mean(percent27$logPenetrate,na.action=na.omit)

se1<-sd(percent1$logPenetrate)/sqrt(length(percent1$logPenetrate))
se3<-sd(percent3$logPenetrate)/sqrt(length(percent3$logPenetrate))
se9<-sd(percent9$logPenetrate)/sqrt(length(percent9$logPenetrate))
se27<-sd(percent27$logPenetrate)/sqrt(length(percent27$logPenetrate))

concentrations<-c(.037,.111,.333,1)
logmeans<-c(mean27,mean9,mean3,mean1)
logseUpper<-c(mean27+se27,mean9+se9,mean3+se3,mean1+se1)
logseLower<-c(mean27-se27,mean9-se9,mean3-se3,mean1-se1)

plot(lipidData$gPerL,lipidData$NumPenetrate)
decay<-function(a,r,x){
	y<-a*exp(r*x)
}
line<-lm(logPenetrate~gPerL,data=lipidData)

model<-nls(props~decay(a,r,x=gPerL),start=c(a=1,r=1),data=block3,control=c(warnOnly=TRUE))
decayPlot<-function(x){
	a<-.77097
	r<-(-.65543)
	y<-a*exp(r*x)
}
curve(decayPlot,from=0,to=1,add=TRUE)
b1<-lm(NumPenetrate~gPerL,data=block1)
b2<-lm(NumPenetrate~gPerL,data=block2)
b3<-lm(NumPenetrate~gPerL,data=block3)
b3mort<-lm(NumMortality~gPerL,data=block3)
b3attract<-lm(NumTotal~gPerL,data=block3)
b3_2<-lm(NumPenetrate~I(gPerL^2),data=block3)

block1<-subset(lipidData,Block==1)
block2<-subset(lipidData,Block==2)
block3<-subset(lipidData,Block==3)
par(mfrow=c(3,1))
plot(block1$gPerL,block1$NumPenetrate)
plot(block2$gPerL,block2$NumPenetrate)
plot(block3$gPerL,block3$NumPenetrate)

model<-lme(NumPenetrate~gPerL,random=~1|Block,data=lipidData)

props<-block3$NumPenetrate/block3$NumTotal
b3<-lm(NumPenetrate~gPerL,data=block3)

tempData<-read.csv(file.choose(),header=TRUE)
plot(tempData$Temperature,tempData$Penetration)

model<-lm(Penetration~Temperature,data=tempData)
model2<-lm(Penetration~I(Temperature^2),data=tempData)