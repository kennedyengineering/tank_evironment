# Tank Game (@kennedyengineering)

# NOTE: For this registry to work correctly, you must use the decorator and import classes in the __init__.py file to ensure the decorator is run.

# Registry dictionary
registry = {}


# Decorator to register agents
def register_agent(cls):
    registry[cls.__name__] = cls
    return cls


# Factory function to create an agent instance using a string key
def create_agent(class_name, *args, **kwargs):
    cls = registry.get(class_name)
    if cls is None:
        raise ValueError(f"Unknown agent class: {class_name}")
    return cls(*args, **kwargs)
