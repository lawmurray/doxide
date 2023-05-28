# Organizing

Doxide organizes documentation by creating a main page, and one additional page for each namespace and type, organized hierarchically. Variables, functions and operators are added to the page of the namespace or class to which they belong. Macros are added to the main page. Only entities with documentation comments are included, with the exception of namespaces, which are included as long as they are non-empty.

Organizing by namespace is not always appropriate. An additional (or alternative) structure may be provided with [groups](#groups). It is also possible to add arbitrary [extra pages](#extra-pages).

## Groups

Groups are for organizing documentation. A hierarchy of groups can be created by adding a  `groups` section to the `doxide.yaml` [configuration file](/configuring). Individual entities can then be assigne to a group by using the `@ingroup` command.

!!! example
    Create a group by adding the following to the `doxide.yaml` configuration file:
    ```yaml
    groups:
      - name: example
        title: Example
        description: Description of the group.
    ```
    Assign an entity to the group using the `@ingroup` command in its documentation comment:
    ```cpp
    /**
     * A function.
     *
     * @ingroup example
     */
    void f();
    ```

The final documentation displays the entity in the group, and not on the main page, a namespace page, or elsewhere. Members of a class may not be assigned to a group (`@ingroup` is simply ignored); they are always included on the associated class page.

If the assignment of groups leaves a namespace empty, it is removed from the final documentation. This allows organizing with groups to completely replace organizing with namespaces.

## Extra pages

Extra pages can be added to the documentation simply by adding extra Markdown files to the output directory (default: `docs`).

Doxide distinguishes its own output files in the output directory by adding `generator: doxide` to the YAML frontmatter. When running the [commands](/command-line) `doxide build` and `doxide clean`, it deletes or overwrites such files without prompting. It will not touch other files.

!!! danger
    Clearly, you do not want to put `generator: doxide` in your extra pages, or they will be deleted!
