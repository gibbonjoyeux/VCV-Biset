
# CONSTRUCTION / DESTRUCTION ORDER
Construction: [Module - dataFromJson - onAdd] - [Widget]
Destruction: [dataToJson], [Widget, onRemove, Module]

# BUGS REGEX

- [ ] Regex: issue with compiled tree initialisation ?
		Sometimes, adding a recursive sequence into a sequence make the
		expression freeze. It then needs to restart from reset input.
