# This file tests creation of a MRML tree containing all node types

catch {load vtktcl}

# Please add all nodes to the list
set nodeTypeList "vtkMrmlVolumeNode vtkMrmlTransformNode"

# Create a tree with all nodes
vtkMrmlTree tree

foreach node $nodeTypeList {

    # unique name
    set name example$node

    # Create the node
    $node $name

    # Note the tree may not have proper syntax, this is just a test
    # Put item on the tree
    tree AddItem $name
}

# Now we can test individual things about the nodes
examplevtkMrmlVolumeNode SetDescription "hello!!!!!!!"


# Now write the file
set filename "test.xml"
tree Write $filename
puts "MRML file saved as $filename"
exit






