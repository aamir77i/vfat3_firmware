################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/echo.c \
../src/iic_phyreset.c \
../src/main.c 

OBJS += \
./src/echo.o \
./src/iic_phyreset.o \
./src/main.o 

C_DEPS += \
./src/echo.d \
./src/iic_phyreset.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O3 -g3 -c -fmessage-length=0 -MT"$@" -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -mxl-frequency -I../../vfat3_sdk_bsp/microblaze_0/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v10.0 -mno-xl-soft-mul -mxl-multiply-high -mhard-float -mxl-float-convert -mxl-float-sqrt -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


