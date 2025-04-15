# Tank Game (@kennedyengineering)

# NOTE: For this registry to work correctly, you must use the decorator and import classes in the __init__.py file to ensure the decorator is run.

# Registry dictionary
registry = {}


# Decorator to register maps
def register_map(cls):
    registry[cls.__name__] = cls
    return cls
