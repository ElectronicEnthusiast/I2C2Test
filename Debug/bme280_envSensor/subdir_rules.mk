################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
bme280_envSensor/%.obj: ../bme280_envSensor/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 -me --include_path="C:/Users/ER/Documents/TI/I2C2Test/include" --include_path="C:/Users/ER/Documents/TI/I2C2Test/bme280_envSensor" --include_path="C:/Users/ER/Documents/TI/I2C2Test" --include_path="C:/Users/ER/Documents/TI/I2C2Test/ov7670" --include_path="C:/Users/ER/Documents/TI/I2C2Test/jpeg-9c" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="bme280_envSensor/$(basename $(<F)).d_raw" --obj_directory="bme280_envSensor" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


