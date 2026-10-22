cmake_minimum_required(VERSION 4.3)
project(simplex_method C)

set(CMAKE_C_STANDARD 11)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/venv")
set(BINARY_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lp_test_A.bin") # just one of the four
set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/lp_test.py")
set(REQUIREMENTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")

# Define platform-independent paths for the venv's python and pip binaries
if(WIN32)
        set(VENV_PYTHON "${VENV_DIR}/Scripts/python.exe")
        set(VENV_PIP "${VENV_DIR}/Scripts/pip.exe")
else()
        set(VENV_PYTHON "${VENV_DIR}/bin/python")
        set(VENV_PIP "${VENV_DIR}/bin/pip")
endif()

# checks if binary already exists, if exists, do not set anything up
if (EXISTS ${BINARY_TEST_FILES})
        message(STATUS "Binary testing files already exist, skipping set up.")
else()
        # check if the virtual environment already exists
        if(EXISTS ${VENV_DIR})
                message(STATUS "Virtual environment already exists in ${VENV_DIR}, skipping creation.")
        else() # if doesn't, generate environment
                # ensure that the given requirements.txt file exists
                if(NOT EXISTS ${REQUIREMENTS_PATH})
                        message(FATAL_ERROR "Requirements file not found: ${REQUIREMENTS_PATH}")
                endif()

                # create the virtual environment
                execute_process(
                        COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_DIR}
                        RESULT_VARIABLE venv_creation_ret_code
                )

                # report error if return code is non-zero
                if(venv_creation_ret_code)
                        message(FATAL_ERROR "Failed to create virtual environment at ${VENV_DIR}!")
                endif()

                # install dependencies from requirements.txt
                execute_process(
                        COMMAND ${VENV_DIR}/bin/pip install -r ${REQUIREMENTS_PATH}
                        RESULT_VARIABLE pip_install_ret_code
                )

                # report error if return code is non-zero
                if(pip_install_ret_code)
                        message(FATAL_ERROR "Failed to install dependencies from ${REQUIREMENTS_PATH}!")
                endif()

                # print success message
                message(STATUS "Virtual environment setup done at ${VENV_DIR} with dependencies from ${REQUIREMENTS_PATH}")

        endif()

        message(STATUS "Running Python script...")

        if(EXISTS "${PYTHON_SCRIPT}")
                execute_process(
                        COMMAND "${VENV_PYTHON}" "${PYTHON_SCRIPT}"
                        RESULT_VARIABLE SCRIPT_RESULT
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                )
                if(SCRIPT_RESULT)
                        message(FATAL_ERROR "Python script execution failed.")
                endif()
        else()
                message(FATAL_ERROR "Python script not found.")
        endif()

endif()



add_executable(SimplexMethod
        main.c
        simplex.c
        simplex.h
        gaussian_elimination.c
        gaussian_elimination.h
        auxiliar_functions.c
        auxiliar_functions.h
        initialize.c
        initialize.h
        phases_1_and_2.c
        phases_1_and_2.h
        )

add_executable(SimplexTests
        test.c
        test.h
        simplex.c
        simplex.h
        gaussian_elimination.c
        gaussian_elimination.h
        auxiliar_functions.c
        auxiliar_functions.h
        initialize.c
        initialize.h
        phases_1_and_2.c
        phases_1_and_2.h
)